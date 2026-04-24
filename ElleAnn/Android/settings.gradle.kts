/*══════════════════════════════════════════════════════════════════════════════
 * settings.gradle.kts — ElleAnn Android companion
 *
 *   Gradle settings for the Android companion app. Uses the Gradle
 *   version catalog (libs.versions.toml) for centralised dependency
 *   version management — keeps Compose BOM, Kotlin, and Android Gradle
 *   Plugin versions in one place for easy bump.
 *══════════════════════════════════════════════════════════════════════════════*/
pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "ElleAnn"
include(":app")
