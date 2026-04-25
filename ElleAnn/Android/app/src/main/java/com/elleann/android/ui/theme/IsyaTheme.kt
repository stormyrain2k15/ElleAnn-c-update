package com.elleann.android.ui.theme

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.material3.Typography
import androidx.compose.material3.Shapes
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.staticCompositionLocalOf
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.foundation.shape.RoundedCornerShape

// ─── Isya Dark Color Scheme (always dark — Isya Night is default) ────────────
private val IsyaDarkColorScheme = darkColorScheme(
    primary            = IsyaGold,
    onPrimary          = IsyaNight,
    primaryContainer   = IsyaGoldDeep,
    onPrimaryContainer = IsyaCream,
    secondary          = IsyaMagic,
    onSecondary        = IsyaNight,
    secondaryContainer = IsyaMagicDeep,
    onSecondaryContainer = IsyaCream,
    tertiary           = ElleViolet,
    onTertiary         = IsyaNight,
    tertiaryContainer  = ElleVioletDeep,
    onTertiaryContainer = IsyaCream,
    background         = IsyaNight,
    onBackground       = IsyaCream,
    surface            = IsyaDusk,
    onSurface          = IsyaCream,
    surfaceVariant     = IsyaMist,
    onSurfaceVariant   = IsyaParchment,
    outline            = IsyaSilverMid,
    outlineVariant     = IsyaSilverDeep,
    error              = IsyaError,
    onError            = IsyaCream,
    errorContainer     = Color(0xFF4A0A0A),
    onErrorContainer   = IsyaCream,
    inverseSurface     = IsyaCream,
    inverseOnSurface   = IsyaNight,
    inversePrimary     = IsyaGoldDeep,
    scrim              = Color(0x99000000),
)

// ─── Typography ──────────────────────────────────────────────────────────────
// Using system fonts in fantasy-adjacent weights; custom fonts can be
// dropped into res/font/ and referenced here via FontFamily(Font(...))
private val IsyaTypography = Typography(
    displayLarge = TextStyle(
        fontFamily = FontFamily.Serif,
        fontWeight = FontWeight.Bold,
        fontSize   = 57.sp,
        lineHeight = 64.sp,
        letterSpacing = (-0.25).sp,
        color = IsyaCream
    ),
    displayMedium = TextStyle(
        fontFamily = FontFamily.Serif,
        fontWeight = FontWeight.SemiBold,
        fontSize   = 45.sp,
        lineHeight = 52.sp,
        color = IsyaCream
    ),
    displaySmall = TextStyle(
        fontFamily = FontFamily.Serif,
        fontWeight = FontWeight.SemiBold,
        fontSize   = 36.sp,
        lineHeight = 44.sp,
        color = IsyaCream
    ),
    headlineLarge = TextStyle(
        fontFamily = FontFamily.Serif,
        fontWeight = FontWeight.SemiBold,
        fontSize   = 32.sp,
        lineHeight = 40.sp,
        color = IsyaGold
    ),
    headlineMedium = TextStyle(
        fontFamily = FontFamily.Serif,
        fontWeight = FontWeight.Medium,
        fontSize   = 28.sp,
        lineHeight = 36.sp,
        color = IsyaGold
    ),
    headlineSmall = TextStyle(
        fontFamily = FontFamily.Serif,
        fontWeight = FontWeight.Medium,
        fontSize   = 24.sp,
        lineHeight = 32.sp,
        color = IsyaGold
    ),
    titleLarge = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.SemiBold,
        fontSize   = 22.sp,
        lineHeight = 28.sp,
        color = IsyaCream
    ),
    titleMedium = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Medium,
        fontSize   = 16.sp,
        lineHeight = 24.sp,
        letterSpacing = 0.15.sp,
        color = IsyaCream
    ),
    titleSmall = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Medium,
        fontSize   = 14.sp,
        lineHeight = 20.sp,
        letterSpacing = 0.1.sp,
        color = IsyaParchment
    ),
    bodyLarge = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Normal,
        fontSize   = 16.sp,
        lineHeight = 24.sp,
        letterSpacing = 0.5.sp,
        color = IsyaCream
    ),
    bodyMedium = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Normal,
        fontSize   = 14.sp,
        lineHeight = 20.sp,
        letterSpacing = 0.25.sp,
        color = IsyaCream
    ),
    bodySmall = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Normal,
        fontSize   = 12.sp,
        lineHeight = 16.sp,
        letterSpacing = 0.4.sp,
        color = IsyaParchment
    ),
    labelLarge = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Medium,
        fontSize   = 14.sp,
        lineHeight = 20.sp,
        letterSpacing = 0.1.sp,
        color = IsyaCream
    ),
    labelMedium = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Medium,
        fontSize   = 12.sp,
        lineHeight = 16.sp,
        letterSpacing = 0.5.sp,
        color = IsyaParchment
    ),
    labelSmall = TextStyle(
        fontFamily = FontFamily.SansSerif,
        fontWeight = FontWeight.Medium,
        fontSize   = 11.sp,
        lineHeight = 16.sp,
        letterSpacing = 0.5.sp,
        color = IsyaMuted
    ),
)

// ─── Shapes ──────────────────────────────────────────────────────────────────
private val IsyaShapes = Shapes(
    extraSmall = RoundedCornerShape(4.dp),
    small      = RoundedCornerShape(8.dp),
    medium     = RoundedCornerShape(12.dp),
    large      = RoundedCornerShape(16.dp),
    extraLarge = RoundedCornerShape(24.dp),
)

// ─── Custom theme extensions ──────────────────────────────────────────────────
data class IsyaExtendedColors(
    val gold: androidx.compose.ui.graphics.Color,
    val goldBright: androidx.compose.ui.graphics.Color,
    val magic: androidx.compose.ui.graphics.Color,
    val magicBright: androidx.compose.ui.graphics.Color,
    val elleViolet: androidx.compose.ui.graphics.Color,
    val cream: androidx.compose.ui.graphics.Color,
    val parchment: androidx.compose.ui.graphics.Color,
    val muted: androidx.compose.ui.graphics.Color,
    val night: androidx.compose.ui.graphics.Color,
    val dusk: androidx.compose.ui.graphics.Color,
    val mist: androidx.compose.ui.graphics.Color,
    val header: androidx.compose.ui.graphics.Color,
    val input: androidx.compose.ui.graphics.Color,
    val silver: androidx.compose.ui.graphics.Color,
    val silverMid: androidx.compose.ui.graphics.Color,
    val silverDeep: androidx.compose.ui.graphics.Color,
    val error: androidx.compose.ui.graphics.Color,
    val success: androidx.compose.ui.graphics.Color,
    val warn: androidx.compose.ui.graphics.Color,
)

private val LocalIsyaColors = staticCompositionLocalOf {
    IsyaExtendedColors(
        gold         = IsyaGold,
        goldBright   = IsyaGoldBright,
        magic        = IsyaMagic,
        magicBright  = IsyaMagicBright,
        elleViolet   = ElleViolet,
        cream        = IsyaCream,
        parchment    = IsyaParchment,
        muted        = IsyaMuted,
        night        = IsyaNight,
        dusk         = IsyaDusk,
        mist         = IsyaMist,
        header       = IsyaHeader,
        input        = IsyaInput,
        silver       = IsyaSilver,
        silverMid    = IsyaSilverMid,
        silverDeep   = IsyaSilverDeep,
        error        = IsyaError,
        success      = IsyaSuccess,
        warn         = IsyaWarn,
    )
}

/** Access Isya custom colors: `IsyaTheme.colors.gold` */
object IsyaTheme {
    val colors: IsyaExtendedColors
        @Composable get() = LocalIsyaColors.current
}

// ─── Theme composable ─────────────────────────────────────────────────────────
@Composable
fun ElleAnnTheme(
    content: @Composable () -> Unit
) {
    val isyaColors = IsyaExtendedColors(
        gold         = IsyaGold,
        goldBright   = IsyaGoldBright,
        magic        = IsyaMagic,
        magicBright  = IsyaMagicBright,
        elleViolet   = ElleViolet,
        cream        = IsyaCream,
        parchment    = IsyaParchment,
        muted        = IsyaMuted,
        night        = IsyaNight,
        dusk         = IsyaDusk,
        mist         = IsyaMist,
        header       = IsyaHeader,
        input        = IsyaInput,
        silver       = IsyaSilver,
        silverMid    = IsyaSilverMid,
        silverDeep   = IsyaSilverDeep,
        error        = IsyaError,
        success      = IsyaSuccess,
        warn         = IsyaWarn,
    )
    CompositionLocalProvider(LocalIsyaColors provides isyaColors) {
        MaterialTheme(
            colorScheme = IsyaDarkColorScheme,
            typography  = IsyaTypography,
            shapes      = IsyaShapes,
            content     = content,
        )
    }
}

// Needed for color scheme references in IsyaColors.kt from this file
private typealias Color = androidx.compose.ui.graphics.Color
