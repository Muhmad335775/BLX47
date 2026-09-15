package com.pulsex.blx47

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.media.MediaMuxer
import java.nio.ByteBuffer

/**
 * Uses the native OS encoder (MediaCodec/MediaMuxer) only - no
 * external third-party video engine dependency, per the Encoding
 * Rule. Produces a solid-color MP4 clip whose color and timing are
 * driven by the branded render descriptor computed in CORE.
 */
class PulseXVideoEncoder {

    fun encodeSolidColorVideo(
        outputPath: String,
        width: Int,
        height: Int,
        fps: Int,
        durationSeconds: Int,
        colorR: Int,
        colorG: Int,
        colorB: Int
    ): Boolean {
        val alignedWidth = width - (width % 16)
        val alignedHeight = height - (height % 16)
        if (alignedWidth <= 0 || alignedHeight <= 0 || fps <= 0 || durationSeconds <= 0) {
            return false
        }

        val format = MediaFormat.createVideoFormat(
            MediaFormat.MIMETYPE_VIDEO_AVC, alignedWidth, alignedHeight
        )
        format.setInteger(
            MediaFormat.KEY_COLOR_FORMAT,
            MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible
        )
        format.setInteger(MediaFormat.KEY_BIT_RATE, alignedWidth * alignedHeight * 4)
        format.setInteger(MediaFormat.KEY_FRAME_RATE, fps)
        format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1)

        val codec = try {
            MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC)
        } catch (e: Exception) {
            return false
        }

        var muxer: MediaMuxer? = null

        return try {
            codec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
            codec.start()

            muxer = MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
            var trackIndex = -1
            var muxerStarted = false

            val totalFrames = fps * durationSeconds
            val frameDurationUs = 1_000_000L / fps
            val ySize = alignedWidth * alignedHeight
            val uvSize = ySize / 4
            val frameData = ByteArray(ySize + uvSize * 2)

            val yValue = rgbToY(colorR, colorG, colorB)
            val uValue = rgbToU(colorR, colorG, colorB)
            val vValue = rgbToV(colorR, colorG, colorB)
            for (i in 0 until ySize) frameData[i] = yValue
            for (i in ySize until ySize + uvSize) frameData[i] = uValue
            for (i in ySize + uvSize until frameData.size) frameData[i] = vValue

            val bufferInfo = MediaCodec.BufferInfo()
            var frameIndex = 0
            var inputDone = false
            var outputDone = false

            while (!outputDone) {
                if (!inputDone) {
                    val inputBufferId = codec.dequeueInputBuffer(10_000)
                    if (inputBufferId >= 0) {
                        val inputBuffer: ByteBuffer? = codec.getInputBuffer(inputBufferId)
                        if (frameIndex < totalFrames) {
                            inputBuffer?.clear()
                            inputBuffer?.put(frameData)
                            codec.queueInputBuffer(
                                inputBufferId, 0, frameData.size,
                                frameIndex * frameDurationUs, 0
                            )
                            frameIndex++
                        } else {
                            codec.queueInputBuffer(
                                inputBufferId, 0, 0, 0,
                                MediaCodec.BUFFER_FLAG_END_OF_STREAM
                            )
                            inputDone = true
                        }
                    }
                }

                val outputBufferId = codec.dequeueOutputBuffer(bufferInfo, 10_000)
                when {
                    outputBufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED -> {
                        trackIndex = muxer.addTrack(codec.outputFormat)
                        muxer.start()
                        muxerStarted = true
                    }
                    outputBufferId >= 0 -> {
                        val outputBuffer = codec.getOutputBuffer(outputBufferId)
                        if (outputBuffer != null && bufferInfo.size > 0 && muxerStarted) {
                            outputBuffer.position(bufferInfo.offset)
                            outputBuffer.limit(bufferInfo.offset + bufferInfo.size)
                            muxer.writeSampleData(trackIndex, outputBuffer, bufferInfo)
                        }
                        codec.releaseOutputBuffer(outputBufferId, false)
                        if (bufferInfo.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM != 0) {
                            outputDone = true
                        }
                    }
                }
            }

            codec.stop()
            codec.release()
            if (muxerStarted) {
                muxer.stop()
            }
            muxer.release()
            true
        } catch (e: Exception) {
            try {
                codec.stop()
            } catch (_: Exception) {
            }
            codec.release()
            try {
                muxer?.release()
            } catch (_: Exception) {
            }
            false
        }
    }

    private fun rgbToY(r: Int, g: Int, b: Int): Byte =
        (((66 * r + 129 * g + 25 * b + 128) shr 8) + 16).toByte()

    private fun rgbToU(r: Int, g: Int, b: Int): Byte =
        (((-38 * r - 74 * g + 112 * b + 128) shr 8) + 128).toByte()

    private fun rgbToV(r: Int, g: Int, b: Int): Byte =
        (((112 * r - 94 * g - 18 * b + 128) shr 8) + 128).toByte()
}
