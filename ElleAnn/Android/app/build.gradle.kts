/*══════════════════════════════════════════════════════════════════════════════
 * app/build.gradle.kts — ElleAnn companion app module
 *
 *   Single-module Android app: Jetpack Compose + OkHttp + Retrofit +
 *   EncryptedSharedPreferences. No Hilt / Dagger — we use a tiny hand-
 *   rolled AppContainer for DI to keep the scaffold readable.
 *══════════════════════════════════════════════════════════════════════════════*/
plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
    id("org.jetbrains.kotlin.plugin.serialization")
}

android {
    namespace  = "com.elleann.android"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.elleann.android"
        minSdk        = 26           /* Android 8.0+ — reliable Keystore */
        targetSdk     = 34
        versionCode   = 1
        versionName   = "0.1.0"
    }

    buildFeatures {
        compose = true
    }

    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.8"
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    kotlinOptions {
        jvmTarget = "17"
    }

    buildTypes {
        debug {
            isMinifyEnabled = false
        }
        release {
            isMinifyEnabled = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    packaging {
        resources {
            excludes += "/META-INF/{AL2.0,LGPL2.1}"
        }
    }
}

dependencies {
    /* Compose BOM — keeps all compose artefacts on matching versions. */
    val composeBom = platform("androidx.compose:compose-bom:2024.02.00")
    implementation(composeBom)
    androidTestImplementation(composeBom)

    implementation("androidx.core:core-ktx:1.12.0")
    implementation("androidx.activity:activity-compose:1.8.2")
    implementation("androidx.lifecycle:lifecycle-runtime-ktx:2.7.0")
    implementation("androidx.lifecycle:lifecycle-viewmodel-compose:2.7.0")

    implementation("androidx.compose.ui:ui")
    implementation("androidx.compose.ui:ui-tooling-preview")
    implementation("androidx.compose.material3:material3")
    implementation("androidx.compose.material:material-icons-extended")

    /* Encrypted persistent storage for the JWT. */
    implementation("androidx.security:security-crypto:1.1.0-alpha06")

    /* Network stack. */
    implementation("com.squareup.okhttp3:okhttp:4.12.0")
    implementation("com.squareup.okhttp3:logging-interceptor:4.12.0")
    implementation("com.squareup.retrofit2:retrofit:2.9.0")
    implementation("com.jakewharton.retrofit:retrofit2-kotlinx-serialization-converter:1.0.0")
    implementation("org.jetbrains.kotlinx:kotlinx-serialization-json:1.6.2")

    /* QR scanning. zxing-android-embedded wraps ZXing with a ready
     * scanner Activity + decoder view.                                */
    implementation("com.journeyapps:zxing-android-embedded:4.3.0")

    debugImplementation("androidx.compose.ui:ui-tooling")
    debugImplementation("androidx.compose.ui:ui-test-manifest")
}
