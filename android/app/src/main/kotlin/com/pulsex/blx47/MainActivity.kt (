package com.pulsex.blx47

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Base64
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.google.android.play.core.integrity.IntegrityManagerFactory
import com.google.android.play.core.integrity.IntegrityTokenRequest
import io.flutter.embedding.android.FlutterActivity
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.plugin.common.EventChannel
import io.flutter.plugin.common.MethodChannel
import java.security.SecureRandom

class MainActivity : FlutterActivity() {

    private val integrityCheckedPrefKey = "blx47_integrity_checked"
    private val audioMethodChannelName = "com.pulsex.blx47/audio"
    private val audioEventChannelName = "com.pulsex.blx47/audio_stream"
    private val recordAudioRequestCode = 4471

    private var audioCapture: PulseXAudioCapture? = null
    private var pendingPermissionResult: MethodChannel.Result? = null
    private var eventSink: EventChannel.EventSink? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        runOneTimeIntegrityCheck()
    }

    override fun configureFlutterEngine(flutterEngine: FlutterEngine) {
        super.configureFlutterEngine(flutterEngine)

        audioCapture = PulseXAudioCapture { chunk ->
            runOnUiThread {
                eventSink?.success(chunk)
            }
        }

        MethodChannel(flutterEngine.dartExecutor.binaryMessenger, audioMethodChannelName)
            .setMethodCallHandler { call, result ->
                when (call.method) {
                    "hasPermission" -> {
                        result.success(hasRecordAudioPermission())
                    }
                    "requestPermission" -> {
                        if (hasRecordAudioPermission()) {
                            result.success(true)
                        } else {
                            pendingPermissionResult = result
                            ActivityCompat.requestPermissions(
                                this,
                                arrayOf(Manifest.permission.RECORD_AUDIO),
                                recordAudioRequestCode
                            )
                        }
                    }
                    "startRecording" -> {
                        if (!hasRecordAudioPermission()) {
                            result.success(false)
                            return@setMethodCallHandler
                        }
                        val sampleRate = call.argument<Int>("sampleRate") ?: 44100
                        val maxDurationSeconds = call.argument<Int>("maxDurationSeconds") ?: 30
                        val started = audioCapture?.start(sampleRate, maxDurationSeconds) ?: false
                        result.success(started)
                    }
                    "stopRecording" -> {
                        val stopped = audioCapture?.stop() ?: false
                        result.success(stopped)
                    }
                    else -> result.notImplemented()
                }
            }

        EventChannel(flutterEngine.dartExecutor.binaryMessenger, audioEventChannelName)
            .setStreamHandler(object : EventChannel.StreamHandler {
                override fun onListen(arguments: Any?, sink: EventChannel.EventSink?) {
                    eventSink = sink
                }

                override fun onCancel(arguments: Any?) {
                    eventSink = null
                }
            })
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == recordAudioRequestCode) {
            val granted = grantResults.isNotEmpty() &&
                grantResults[0] == PackageManager.PERMISSION_GRANTED
            pendingPermissionResult?.success(granted)
            pendingPermissionResult = null
        }
    }

    private fun hasRecordAudioPermission(): Boolean {
        return ContextCompat.checkSelfPermission(
            this,
            Manifest.permission.RECORD_AUDIO
        ) == PackageManager.PERMISSION_GRANTED
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

    override fun onDestroy() {
        audioCapture?.stop()
        super.onDestroy()
    }
}
