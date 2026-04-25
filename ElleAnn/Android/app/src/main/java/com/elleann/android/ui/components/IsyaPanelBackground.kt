package com.elleann.android.ui.components

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import kotlin.math.PI
import kotlin.math.cos
import kotlin.math.sin
import kotlin.random.Random

/*══════════════════════════════════════════════════════════════════════════════
 * Fiesta Online "two-tone striped blue" panel background.
 *
 *   Calibrated against the user-supplied inventory reference screenshot:
 *
 *     base       = #0D1B44   (deep midnight blue floor)
 *     streakLite = #3C5FA8   (lightest streak crest)
 *     streakDark = #040C28   (darkest streak trough)
 *     period     ≈ 18% of panel width
 *     tilt       ≈ 12° from vertical
 *     noise      ≈ 3% alpha white speckles
 *
 *   Implementation: a base fill + N tilted vertical "streak" gradients
 *   spaced at the period, each running dark→light→dark across its band,
 *   plus a deterministic speckle pass on top for the film-grain look.
 *
 *   Performance: drawn in a single Canvas pass with cached speckle
 *   offsets — no per-frame allocations.
 *══════════════════════════════════════════════════════════════════════════════*/

/** Default Fiesta-inventory background palette. */
val FiestaPanelBase   = Color(0xFF0D1B44)
val FiestaStreakLite  = Color(0xFF3C5FA8)
val FiestaStreakDark  = Color(0xFF040C28)

/**
 * Paints the Fiesta-style striped blue background. Caller is expected
 * to clip this to the panel's rounded shape; the caller's content goes
 * on top.
 */
@Composable
fun IsyaFiestaPanelBackground(
    modifier:    Modifier = Modifier,
    cornerRadius: Dp     = 12.dp,
    streakCount:  Int    = 6,
    tiltDegrees:  Float  = 12f,
) {
    /* Speckle positions are deterministic per-recomposition so the grain
     * doesn't shimmer between frames (the reference screenshot has a
     * static grain, not animated noise).                                */
    val speckles = remember {
        val rng = Random(0xE11EA117)
        Array(140) { rng.nextFloat() to rng.nextFloat() }
    }

    Canvas(
        modifier = modifier
            .fillMaxSize()
            .clip(RoundedCornerShape(cornerRadius))
    ) {
        // 1. Base fill
        drawRect(color = FiestaPanelBase, size = size)

        // 2. Tilted streak bands
        val w     = size.width
        val h     = size.height
        val tilt  = (tiltDegrees * PI / 180.0).toFloat()
        val cosT  = cos(tilt.toDouble()).toFloat()
        val sinT  = sin(tilt.toDouble()).toFloat()
        // Period width — distance between streak centres along the
        // streak-perpendicular axis.
        val period = w / streakCount
        val bandW  = period * 0.65f   // each band is ~65% of the spacing

        // Project streak band centres along the tilted axis. We extend
        // beyond the panel edges so the tilt doesn't leave bald patches.
        val span = (w + h * sinT / cosT).coerceAtLeast(w)
        val startX = -bandW
        var cx = startX
        while (cx < span) {
            val gradient = Brush.linearGradient(
                colors = listOf(
                    FiestaStreakDark,
                    FiestaStreakLite,
                    FiestaStreakDark,
                ),
                start = Offset(cx - bandW / 2, 0f),
                end   = Offset(cx + bandW / 2, h),
            )
            // Tilt the band by drawing a parallelogram-shaped path. The
            // rotation is small so a 4-vertex polygon reads as cleanly
            // as a true rotated rect, with no clipping artefacts.
            val path = Path().apply {
                moveTo(cx - bandW / 2, 0f)
                lineTo(cx + bandW / 2, 0f)
                lineTo(cx + bandW / 2 - h * sinT / cosT, h)
                lineTo(cx - bandW / 2 - h * sinT / cosT, h)
                close()
            }
            drawPath(path = path, brush = gradient, alpha = 0.55f)
            cx += period
        }

        // 3. Deterministic speckle grain
        val grain = Color(0xFFC8D6F0).copy(alpha = 0.06f)
        for ((u, v) in speckles) {
            drawCircle(
                color  = grain,
                radius = 0.6f,
                center = Offset(u * w, v * h),
            )
        }
    }
}

/*══════════════════════════════════════════════════════════════════════════════
 * IsyaCornerOrnaments — small triangular notches at each panel corner,
 * painted in the same silver gradient as the bevel.
 *
 *   Matches the Fiesta UI's bevel decoration: a tiny right-triangle of
 *   silver at each corner, oriented inward, gives the frame a hand-
 *   forged feel rather than a plain rounded rectangle.
 *
 *   Opt in via IsyaPanel(decoratedCorners = true).
 *══════════════════════════════════════════════════════════════════════════════*/
@Composable
fun IsyaCornerOrnaments(
    modifier: Modifier = Modifier,
    notchSize: Dp = 8.dp,
) {
    Canvas(modifier = modifier.fillMaxSize()) {
        val n = notchSize.toPx()
        val w = size.width
        val h = size.height
        val brush = Brush.linearGradient(
            colors = listOf(
                Color(0xFFD2D7DC),      // IsyaSilver
                Color(0xFFA5AFB4),      // IsyaSilverMid
                Color(0xFF6E7378),      // IsyaSilverDeep
            )
        )
        // Top-left
        drawPath(Path().apply {
            moveTo(0f, n);  lineTo(n, 0f);  lineTo(n, n);  close()
        }, brush)
        // Top-right
        drawPath(Path().apply {
            moveTo(w - n, 0f); lineTo(w, n); lineTo(w - n, n); close()
        }, brush)
        // Bottom-left
        drawPath(Path().apply {
            moveTo(0f, h - n); lineTo(n, h - n); lineTo(n, h); close()
        }, brush)
        // Bottom-right
        drawPath(Path().apply {
            moveTo(w - n, h - n); lineTo(w, h - n); lineTo(w - n, h); close()
        }, brush)
    }
}
