package com.pulsex.blx47

import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioRecord
import android.media.MediaRecorder
import android.media.ToneGenerator
import java.util.concurrent.atomic.AtomicBoolean
import kotlin.concurrent.thread

class PulseXAudioCapture(
    private val onChunk: (DoubleArray) -> Unit
) {
    private val isRecording = AtomicBoolean(false)
    private var recordThread: Thread? = null
    private var audioRecord: AudioRecord? = null
    private val toneGenerator = ToneGenerator(AudioManager.STREAM_MUSIC, 80)

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
        }

        if (record.state != AudioRecord.STATE_INITIALIZED) {
            record.release()
            return false
        }

        audioRecord = record
        isRecording.set(true)
        toneGenerator.startTone(ToneGenerator.TONE_PROP_BEEP, 120)
        record.startRecording()

        val maxSamples = sampleRate.toLong() * maxDurationSeconds.toLong()
        var samplesRead = 0L

        recordThread = thread(start = true) {
            val shortBuffer = ShortArray(minBufferSize)
            while (isRecording.get()) {
                val read = record.read(shortBuffer, 0, shortBuffer.size)
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

        toneGenerator.startTone(ToneGenerator.TONE_PROP_ACK, 120)
        return true
    }
}
