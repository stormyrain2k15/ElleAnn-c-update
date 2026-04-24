package com.elleann.android.ui.theme

import android.app.Activity
import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.dynamicDarkColorScheme
import androidx.compose.material3.dynamicLightColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.SideEffect
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalView
import androidx.core.view.WindowCompat

/* Muted teal/umber palette — avoids the generic purple-Compose default.
 * Tweak freely; this is the minimal scaffold, not a design-system pass. */
private val ElleLight = lightColorScheme(
    primary        = Color(0xFF3A6C72),
    onPrimary      = Color.White,
    secondary      = Color(0xFF8F5D3A),
    background     = Color(0xFFFAF7F2),
    surface        = Color(0xFFFAF7F2),
    onBackground   = Color(0xFF1C1B1A),
    onSurface      = Color(0xFF1C1B1A),
)

private val ElleDark = darkColorScheme(
    primary        = Color(0xFF7BB6BC),
    onPrimary      = Color(0xFF04363C),
    secondary      = Color(0xFFE0B893),
    background     = Color(0xFF121212),
    surface        = Color(0xFF1A1A1A),
    onBackground   = Color(0xFFE8E6E3),
    onSurface      = Color(0xFFE8E6E3),
)

@Composable
fun ElleTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    dynamicColor: Boolean = true,
    content: @Composable () -> Unit,
) {
    val colors = when {
        dynamicColor && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> {
            val ctx = LocalContext.current
            if (darkTheme) dynamicDarkColorScheme(ctx)
            else           dynamicLightColorScheme(ctx)
        }
        darkTheme -> ElleDark
        else      -> ElleLight
    }
    val view = LocalView.current
    if (!view.isInEditMode) {
        SideEffect {
            val window = (view.context as Activity).window
            window.statusBarColor = colors.background.toArgb()
            WindowCompat.getInsetsController(window, view)
                .isAppearanceLightStatusBars = !darkTheme
        }
    }
    MaterialTheme(colorScheme = colors, content = content)
}
