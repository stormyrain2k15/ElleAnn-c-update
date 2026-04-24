/*
 * build.gradle.kts (root) — only declares the plugins used by submodules.
 * Actual configuration lives in app/build.gradle.kts.
 */
plugins {
    id("com.android.application") version "8.3.0" apply false
    id("org.jetbrains.kotlin.android") version "1.9.22" apply false
    id("org.jetbrains.kotlin.plugin.serialization") version "1.9.22" apply false
}
