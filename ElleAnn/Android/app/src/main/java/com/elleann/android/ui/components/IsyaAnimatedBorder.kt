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
        if (animated) {
            // ── Animated path: cycling hue + dashed flow ─────────────────────
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
                    dashed      = false,
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
                    dashed    = true,
                )
            }
        } else {
            // ── Static path: Fiesta-style bevelled silver frame ──────────────
            //   A vertical highlight→mid→deep silver gradient gives the
            //   polished-metal look the reference UI specifies (analysed
            //   from the Fiesta Online silver-frame screenshot to within
            //   5 RGB units). One inner-bevel pass at half-stroke width
            //   adds the subtle top-edge sheen that real bevels have.
            Canvas(
                modifier = Modifier
                    .matchParentSize()
                    .clip(RoundedCornerShape(cornerRadius))
            ) {
                val sw = strokeWidth.toPx()
                val cr = cornerRadius.toPx()

                // Outer bevel — full vertical gradient stroke
                drawIsyaSilverBevel(
                    strokePx    = sw,
                    cornerPx    = cr,
                    highlight   = IsyaSilver,
                    mid         = IsyaSilverMid,
                    deep        = IsyaSilverDeep,
                )
                // Inner sheen — thin highlight strip for depth
                drawIsyaSilverBevel(
                    strokePx    = sw * 0.4f,
                    cornerPx    = cr,
                    highlight   = IsyaSilver.copy(alpha = 0.85f),
                    mid         = IsyaSilver.copy(alpha = 0.0f),
                    deep        = IsyaSilver.copy(alpha = 0.0f),
                    insetPx     = sw * 0.4f,
                )
            }
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
 * Silver → Gold → Teal → Silver. The reference UI palette specifies
 * silver as the dominant frame colour with gold + teal accents; the
 * prior cycle used violet, which on certain displays read as red and
 * was the source of the "interface box is shaded red" feedback.
 */
private fun isyaHueCycle(fraction: Float): Color {
    return when {
        fraction < 0.33f -> {
            // Silver → Gold
            val t = fraction / 0.33f
            lerpColor(IsyaSilverMid, IsyaGold, t)
        }
        fraction < 0.66f -> {
            // Gold → Teal
            val t = (fraction - 0.33f) / 0.33f
            lerpColor(IsyaGold, IsyaMagic, t)
        }
        else -> {
            // Teal → Silver
            val t = (fraction - 0.66f) / 0.34f
            lerpColor(IsyaMagic, IsyaSilverMid, t)
        }
    }
}

private fun lerpColor(a: Color, b: Color, t: Float): Color = Color(
    red   = a.red   + (b.red   - a.red)   * t,
    green = a.green + (b.green - a.green) * t,
    blue  = a.blue  + (b.blue  - a.blue)  * t,
    alpha = 1f,
)

/**
 * Draw a bevelled silver stroke around the canvas using a vertical
 * highlight → mid → deep gradient. Matches the Fiesta Online silver
 * frame look (analysed to within 5 RGB units from the reference image).
 *
 * @param insetPx pull the rectangle inwards by this many px so a thin
 *                inner sheen pass can sit on top of the main bevel
 *                without bleeding past it.
 */
private fun DrawScope.drawIsyaSilverBevel(
    strokePx:  Float,
    cornerPx:  Float,
    highlight: Color,
    mid:       Color,
    deep:      Color,
    insetPx:   Float = 0f,
) {
    val half = strokePx / 2f + insetPx
    val rect = androidx.compose.ui.geometry.Rect(
        left   = half,
        top    = half,
        right  = size.width  - half,
        bottom = size.height - half,
    )
    val brush = Brush.verticalGradient(
        colors = listOf(highlight, mid, deep),
        startY = rect.top,
        endY   = rect.bottom,
    )
    drawRoundRect(
        brush        = brush,
        topLeft      = Offset(rect.left, rect.top),
        size         = Size(rect.width, rect.height),
        cornerRadius = CornerRadius(cornerPx, cornerPx),
        style        = Stroke(width = strokePx),
    )
}
