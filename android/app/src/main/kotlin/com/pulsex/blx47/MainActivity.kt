package com.pulsex.blx47

import android.os.Bundle
import android.util.Base64
import com.google.android.play.core.integrity.IntegrityManagerFactory
import com.google.android.play.core.integrity.IntegrityTokenRequest
import io.flutter.embedding.android.FlutterActivity
import java.security.SecureRandom

class MainActivity : FlutterActivity() {

    private val integrityCheckedPrefKey = "blx47_integrity_checked"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        runOneTimeIntegrityCheck()
    }

    private fun runOneTimeIntegrityCheck() {
        val prefs = getSharedPreferences("blx47_prefs", MODE_PRIVATE)
        if (prefs.getBoolean(integrityCheckedPrefKey, false)) {
            return
        }

        val integrityManager = IntegrityManagerFactory.create(applicationContext)
        val request = IntegrityTokenRequest.builder()
            .setCloudProjectNumber(BuildConfig.BLX47_CLOUD_PROJECT_NUMBER)
            .setNonce(generateNonce())
            .build()

        integrityManager.requestIntegrityToken(request)
            .addOnSuccessListener {
                prefs.edit().putBoolean(integrityCheckedPrefKey, true).apply()
            }
            .addOnFailureListener {
                // Retry is deferred to next launch; app remains usable offline.
            }
    }

    private fun generateNonce(): String {
        val bytes = ByteArray(16)
        SecureRandom().nextBytes(bytes)
        return Base64.encodeToString(
            bytes,
            Base64.URL_SAFE or Base64.NO_WRAP or Base64.NO_PADDING
        )
    }
}
