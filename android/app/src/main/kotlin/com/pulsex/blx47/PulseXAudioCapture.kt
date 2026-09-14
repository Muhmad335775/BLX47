package com.pulsex.blx47

import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioRecord
import android.media.MediaRecorder
import android.media.ToneGenerator
import android.util.Log
import java.util.concurrent.atomic.AtomicBoolean
import kotlin.concurrent.thread

class PulseXAudioCapture(
    private val onChunk: (DoubleArray) -> Unit
) {
    private val isRecording = AtomicBoolean(false)
    private var recordThread: Thread? = null
    private var audioRecord: AudioRecord? = null

    private val toneGenerator: ToneGenerator? = try {
        ToneGenerator(AudioManager.STREAM_MUSIC, 80)
    } catch (e: RuntimeException) {
        Log.w("PulseXAudioCapture", "ToneGenerator unavailable: ${e.message}")
        null
    }

    fun start(sampleRate: Int, maxDurationSeconds: Int): Boolean {
        if (isRecording.get()) {
            return false
        }

        val minBufferSize = AudioRecord.getMinBufferSize(
            sampleRate,
            AudioFormat.CHANNEL_IN_MONO,
            AudioFormat.ENCODING_PCM_16BIT
        )
        if (minBufferSize <= 0) {
            return false
        }

        val record = try {
            AudioRecord(
                MediaRecorder.AudioSource.MIC,
                sampleRate,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT,
                minBufferSize * 2
            )
        } catch (e: SecurityException) {
            return false
        } catch (e: RuntimeException) {
            return false
        }

        if (record.state != AudioRecord.STATE_INITIALIZED) {
            record.release()
            return false
        }

        audioRecord = record
        isRecording.set(true)

        try {
            toneGenerator?.startTone(ToneGenerator.TONE_PROP_BEEP, 120)
        } catch (e: RuntimeException) {
            Log.w("PulseXAudioCapture", "startTone failed: ${e.message}")
        }

        try {
            record.startRecording()
        } catch (e: IllegalStateException) {
            isRecording.set(false)
            audioRecord = null
            record.release()
            return false
        }

        val maxSamples = sampleRate.toLong() * maxDurationSeconds.toLong()
        var samplesRead = 0L

        recordThread = thread(start = true) {
            val shortBuffer = ShortArray(minBufferSize)
            while (isRecording.get()) {
                val read = try {
                    record.read(shortBuffer, 0, shortBuffer.size)
                } catch (e: Exception) {
                    -1
                }
                if (read > 0) {
                    val doubleChunk = DoubleArray(read)
                    for (i in 0 until read) {
                        doubleChunk[i] = shortBuffer[i] / 32768.0
                    }
                    onChunk(doubleChunk)
                    samplesRead += read
                    if (samplesRead >= maxSamples) {
                        isRecording.set(false)
                    }
                }
            }
        }

        return true
    }

    fun stop(): Boolean {
        if (!isRecording.get() && audioRecord == null) {
            return false
        }
        isRecording.set(false)
        recordThread?.join(500)
        recordThread = null

        try {
            audioRecord?.stop()
        } catch (e: IllegalStateException) {
            // Already stopped; safe to ignore.
        }
        audioRecord?.release()
        audioRecord = null

        try {
            toneGenerator?.startTone(ToneGenerator.TONE_PROP_ACK, 120)
        } catch (e: RuntimeException) {
            Log.w("PulseXAudioCapture", "stop tone failed: ${e.message}")
        }

        return true
    }
}
