-keep class com.pulsex.blx47.** { *; }
-keepclasseswithmembers class * {
    native <methods>;
}
-keep class com.google.android.play.core.integrity.** { *; }
-dontwarn com.google.android.play.core.integrity.**
-optimizationpasses 5
-allowaccessmodification
-repackageclasses ''
