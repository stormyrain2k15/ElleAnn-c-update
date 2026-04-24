package com.elleann.android

import android.app.Application
import com.elleann.android.data.AppContainer

/**
 * ElleApp — process-wide singletons.
 *
 *   Holds the [AppContainer], which owns:
 *     - the [TokenStore] backed by EncryptedSharedPreferences,
 *     - the configured OkHttp client with the [AuthInterceptor] attached,
 *     - the Retrofit service bound to the currently-paired host.
 *
 *   Keeping it in [Application.onCreate] means the first screen never
 *   has to do a synchronous Keystore unlock — it's already done by the
 *   time the first composition runs.
 */
class ElleApp : Application() {
    lateinit var container: AppContainer
        private set

    override fun onCreate() {
        super.onCreate()
        container = AppContainer(applicationContext)
    }
}
