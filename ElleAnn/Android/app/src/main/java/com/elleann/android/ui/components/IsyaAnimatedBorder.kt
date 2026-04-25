package com.elleann.android.ui.components

import androidx.compose.animation.core.*
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import com.elleann.android.ui.theme.*
import kotlin.math.PI
import kotlin.math.cos
import kotlin.math.sin

/**
 * IsyaAnimatedBorder — pure Compose Canvas implementation of the Isya flowing
 * border effect. Replaces the Unreal Engine flipbook texture entirely.
 *
 * Two simultaneous animations run in opposite directions so they never sync:
 *   - Dash phase scrolls LEFT (border paint moves along the perimeter)
 *   - Hue shifts RIGHT (gold → teal → violet → gold, ~40s full cycle)
 *
 * Emission glow: a second wider stroke at 20% alpha behind the main stroke.
 *
 * This wraps any composable content in the Isya panel frame.
 */
@Composable
fun IsyaAnimatedBorderBox(
    modifier: Modifier = Modifier,
    cornerRadius: Dp = 12.dp,
    strokeWidth: Dp = 1.5.dp,
    glowWidth: Dp = 6.dp,
    animated: Boolean = true,
    content: @Composable BoxScope.() -> Unit,
) {
    val infiniteTransition = rememberInfiniteTransition(label = "isya_border")

    // Dash phase — scrolls left. Full perimeter travel time ~50s
    val dashPhase by if (animated) {
        infiniteTransition.animateFloat(
            initialValue  = 0f,
            targetValue   = 1000f,
            animationSpec = infiniteRepeatable(
                animation  = tween(50_000, easing = LinearEasing),
                repeatMode = RepeatMode.Restart,
            ),
            label = "dash_phase",
        )
    } else { remember { mutableStateOf(0f) } }

    // Hue fraction — cycles right. Full color rotation ~40s
    val hueFraction by if (animated) {
        infiniteTransition.animateFloat(
            initialValue  = 0f,
            targetValue   = 1f,
            animationSpec = infiniteRepeatable(
                animation  = tween(40_000, easing = LinearEasing),
                repeatMode = RepeatMode.Restart,
            ),
            label = "hue_fraction",
        )
    } else { remember { mutableStateOf(0f) } }

    // Compute current border color from hue fraction
    // Cycle: gold(0) → teal(0.33) → violet(0.66) → gold(1.0)
    val borderColor = isyaHueCycle(hueFraction)

    Box(modifier = modifier) {
        // Glow layer (wide, low alpha)
        Canvas(
            modifier = Modifier
                .matchParentSize()
                .clip(RoundedCornerShape(cornerRadius))
        ) {
            drawIsyaBorder(
                color       = borderColor.copy(alpha = 0.20f),
                strokePx    = glowWidth.toPx(),
                cornerPx    = cornerRadius.toPx(),
                dashPhase   = dashPhase,
                dashed      = false, // glow is solid
            )
        }

        // Main animated dashed border
        Canvas(
            modifier = Modifier
                .matchParentSize()
                .clip(RoundedCornerShape(cornerRadius))
        ) {
            drawIsyaBorder(
                color     = borderColor,
                strokePx  = strokeWidth.toPx(),
                cornerPx  = cornerRadius.toPx(),
                dashPhase = dashPhase,
                dashed    = animated,
            )
        }

        // Content inside the box
        Box(
            modifier = Modifier
                .fillMaxSize()
                .padding(strokeWidth),
            content = content,
        )
    }
}

/**
 * Draw the Isya border path around the box — rounded rect with optional
 * running dash effect.
 */
private fun DrawScope.drawIsyaBorder(
    color: Color,
    strokePx: Float,
    cornerPx: Float,
    dashPhase: Float,
    dashed: Boolean,
) {
    val halfStroke = strokePx / 2f
    val rect = androidx.compose.ui.geometry.Rect(
        left   = halfStroke,
        top    = halfStroke,
        right  = size.width  - halfStroke,
        bottom = size.height - halfStroke,
    )

    val pathEffect = if (dashed) {
        PathEffect.dashPathEffect(
            intervals = floatArrayOf(18f, 10f),
            phase     = -dashPhase, // negative = scrolls left
        )
    } else null

    drawRoundRect(
        color       = color,
        topLeft     = Offset(rect.left, rect.top),
        size        = Size(rect.width, rect.height),
        cornerRadius = CornerRadius(cornerPx, cornerPx),
        style       = Stroke(
            width      = strokePx,
            pathEffect = pathEffect,
        ),
    )
}

/**
 * Isya hue cycle: maps a 0..1 fraction to a color cycling through
 * Gold → Teal → Violet → Gold, matching the original Unreal material spec.
 */
private fun isyaHueCycle(fraction: Float): Color {
    return when {
        fraction < 0.33f -> {
            // Gold → Teal
            val t = fraction / 0.33f
            lerpColor(IsyaGold, IsyaMagic, t)
        }
        fraction < 0.66f -> {
            // Teal → Violet
            val t = (fraction - 0.33f) / 0.33f
            lerpColor(IsyaMagic, ElleViolet, t)
        }
        else -> {
            // Violet → Gold
            val t = (fraction - 0.66f) / 0.34f
            lerpColor(ElleViolet, IsyaGold, t)
        }
    }
}

private fun lerpColor(a: Color, b: Color, t: Float): Color = Color(
    red   = a.red   + (b.red   - a.red)   * t,
    green = a.green + (b.green - a.green) * t,
    blue  = a.blue  + (b.blue  - a.blue)  * t,
    alpha = 1f,
)
