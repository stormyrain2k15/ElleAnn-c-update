package com.elleann.android.ui.components

import com.elleann.android.R

/*══════════════════════════════════════════════════════════════════════════════
 * IsyaSpriteCatalog — named UV regions inside the bundled Fiesta atlases.
 *
 *   These names are the source of truth for "where in the atlas does
 *   <thing> live". Call-sites should reference [SpriteRegion] entries
 *   so a future atlas re-pack or coordinate tweak is a one-line change.
 *
 *   The four atlases (drawable IDs in [Atlas]) ship at the original
 *   sizes from the user's TGA uploads:
 *     ATLAS_LARGE  = elle_textures_atlas       (512×512)
 *     ATLAS_MED    = elle_textures_atlas_med   (256×256)
 *     ATLAS_SMALL  = elle_textures_atlas_small (128×128)
 *     ATLAS_XS     = elle_textures_atlas_xs    (32×32)
 *
 *   The exact UV coordinates below are placeholders centred on the
 *   atlas — they MUST be updated to match the actual sprite positions
 *   after eyeballing the atlases. The structure is correct; the
 *   numbers are the only thing that needs tuning. Update one line per
 *   sprite as you identify each one.
 *══════════════════════════════════════════════════════════════════════════════*/

/** Stable handle for one of the 4 bundled atlases. */
enum class Atlas(val drawableId: Int, val sizePx: Int) {
    LARGE (R.drawable.elle_textures_atlas,        512),
    MED   (R.drawable.elle_textures_atlas_med,    256),
    SMALL (R.drawable.elle_textures_atlas_small,  128),
    XS    (R.drawable.elle_textures_atlas_xs,      32),
}

/** A named (x, y, w, h) region inside an atlas. */
data class SpriteRegion(
    val atlas: Atlas,
    val x: Int,
    val y: Int,
    val w: Int,
    val h: Int,
)

/**
 * Named regions. These are PLACEHOLDERS — every coordinate below is the
 * top-left quadrant of the relevant atlas. After visually inspecting
 * each atlas, replace these with the real UVs for each sprite. The
 * surrounding code does not need to change.
 */
object IsyaSprites {

    // ── Buttons ─────────────────────────────────────────────────────────────
    val BTN_OK_NORMAL  = SpriteRegion(Atlas.SMALL,  0,  0, 64, 32)
    val BTN_OK_HOVER   = SpriteRegion(Atlas.SMALL, 64,  0, 64, 32)
    val BTN_CANCEL     = SpriteRegion(Atlas.SMALL,  0, 32, 64, 32)
    val BTN_CIRCLE     = SpriteRegion(Atlas.SMALL, 64, 32, 32, 32)

    // ── Status-bar pieces ───────────────────────────────────────────────────
    val BAR_HP_FILL    = SpriteRegion(Atlas.MED,   0,   0, 128, 16)
    val BAR_HP_FRAME   = SpriteRegion(Atlas.MED,   0,  16, 128, 16)
    val BAR_MP_FILL    = SpriteRegion(Atlas.MED,   0,  32, 128, 16)
    val BAR_MP_FRAME   = SpriteRegion(Atlas.MED,   0,  48, 128, 16)
    val BAR_XP_FILL    = SpriteRegion(Atlas.MED,   0,  64, 128, 16)

    // ── Inventory slots ─────────────────────────────────────────────────────
    val SLOT_EMPTY     = SpriteRegion(Atlas.SMALL,  0, 64, 32, 32)
    val SLOT_HIGHLIGHT = SpriteRegion(Atlas.SMALL, 32, 64, 32, 32)

    // ── Frame corners (Fiesta-style ornamental notches) ─────────────────────
    val CORNER_TL      = SpriteRegion(Atlas.MED, 128,   0, 32, 32)
    val CORNER_TR      = SpriteRegion(Atlas.MED, 160,   0, 32, 32)
    val CORNER_BL      = SpriteRegion(Atlas.MED, 128,  32, 32, 32)
    val CORNER_BR      = SpriteRegion(Atlas.MED, 160,  32, 32, 32)

    // ── Status icons (small) ────────────────────────────────────────────────
    val DOT_CONNECTED  = SpriteRegion(Atlas.XS,    0,  0, 16, 16)
    val DOT_SLOW       = SpriteRegion(Atlas.XS,   16,  0, 16, 16)
    val DOT_OFFLINE    = SpriteRegion(Atlas.XS,    0, 16, 16, 16)
}
