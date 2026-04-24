# ProGuard rules — keep kotlinx.serialization-generated @Serializable
# classes intact after R8. The Retrofit converter reflects on them.

-keepattributes *Annotation*, InnerClasses
-dontnote kotlinx.serialization.AnnotationsKt

-keep,includedescriptorclasses class com.elleann.android.data.** {
    *;
}
-keepclassmembers class com.elleann.android.** {
    *** Companion;
}
-keepclasseswithmembers class com.elleann.android.** {
    kotlinx.serialization.KSerializer serializer(...);
}

# OkHttp / Retrofit are well-behaved under R8 with their own consumer
# rules; no extra keeps needed.
