package com.elleann.android.ui.components

import androidx.annotation.DrawableRes
import androidx.compose.foundation.Image
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.painter.BitmapPainter
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.imageResource
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize

/*══════════════════════════════════════════════════════════════════════════════
 * IsyaSprite — render a sub-region of a packed texture atlas.
 *
 *   The bundled Fiesta texture atlases (`R.drawable.elle_textures_atlas*`)
 *   contain dozens of sprites — buttons, status-bar pieces, slot wells,
 *   icons, loading-bar caps, corner notches. Pre-cropping each into its
 *   own drawable would mean churning out ~40 separate PNG files and
 *   keeping their dimensions in sync with the atlas. Instead, this
 *   composable takes UV coordinates (in pixels, top-left origin) and
 *   renders just that region.
 *
 *   Usage:
 *
 *       IsyaSprite(
 *           atlas        = R.drawable.elle_textures_atlas,
 *           x = 64, y = 128, w = 32, h = 32,
 *           modifier     = Modifier.size(48.dp),       // upscale 1.5×
 *           contentScale = ContentScale.Fit,
 *       )
 *
 *   The bitmap is loaded once and cached by the underlying
 *   [imageResource] caller; subsequent IsyaSprite() calls referencing
 *   the same atlas reuse the cached bitmap with zero allocation.
 *══════════════════════════════════════════════════════════════════════════════*/
@Composable
fun IsyaSprite(
    @DrawableRes atlas: Int,
    x: Int,
    y: Int,
    w: Int,
    h: Int,
    modifier: Modifier = Modifier,
    contentDescription: String? = null,
    contentScale: ContentScale = ContentScale.Fit,
) {
    val bitmap = androidx.compose.ui.res.imageResource(atlas)
    val painter = remember(bitmap, x, y, w, h) {
        /* Clamp the requested region to the atlas bounds so a typo in
         * UV coordinates can't crash the renderer — unmapped pixels
         * end up as the atlas's transparent background.              */
        val sx = x.coerceIn(0, bitmap.width  - 1)
        val sy = y.coerceIn(0, bitmap.height - 1)
        val sw = w.coerceAtMost(bitmap.width  - sx).coerceAtLeast(1)
        val sh = h.coerceAtMost(bitmap.height - sy).coerceAtLeast(1)
        BitmapPainter(
            image     = bitmap,
            srcOffset = IntOffset(sx, sy),
            srcSize   = IntSize(sw, sh),
        )
    }
    Image(
        painter            = painter,
        contentDescription = contentDescription,
        modifier           = modifier,
        contentScale       = contentScale,
    )
}

/**
 * Convenience overload for catalogued sprites — call sites read as
 * `IsyaSprite(IsyaSprites.BTN_OK_NORMAL, modifier = Modifier.size(48.dp))`
 * with no magic numbers.
 */
@Composable
fun IsyaSprite(
    region: SpriteRegion,
    modifier: Modifier = Modifier,
    contentDescription: String? = null,
    contentScale: ContentScale = ContentScale.Fit,
) = IsyaSprite(
    atlas              = region.atlas.drawableId,
    x                  = region.x,
    y                  = region.y,
    w                  = region.w,
    h                  = region.h,
    modifier           = modifier,
    contentDescription = contentDescription,
    contentScale       = contentScale,
)
