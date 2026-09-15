import 'package:flutter/services.dart';

class VideoExport {
  VideoExport._();
  static final VideoExport instance = VideoExport._();

  static const MethodChannel _channel =
      MethodChannel('com.pulsex.blx47/video');

  /// Calls the native OS encoder (MediaCodec/MediaMuxer) to produce
  /// a branded MP4 file. Returns the output file path, or null on
  /// failure.
  Future<String?> encodeSolidColorVideo({
    required int width,
    required int height,
    required int fps,
    required int durationSeconds,
    required int colorR,
    required int colorG,
    required int colorB,
  }) async {
    final result = await _channel.invokeMethod<String>('encodeVideo', {
      'width': width,
      'height': height,
      'fps': fps,
      'durationSeconds': durationSeconds,
      'colorR': colorR,
      'colorG': colorG,
      'colorB': colorB,
    });
    return result;
  }
}
