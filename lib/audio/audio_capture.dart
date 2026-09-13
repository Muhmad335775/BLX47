import 'package:flutter/services.dart';

class AudioCapture {
  AudioCapture._();
  static final AudioCapture instance = AudioCapture._();

  static const MethodChannel _methodChannel =
      MethodChannel('com.pulsex.blx47/audio');
  static const EventChannel _eventChannel =
      EventChannel('com.pulsex.blx47/audio_stream');

  Stream<List<double>>? _chunkStream;

  Future<bool> hasPermission() async {
    final result = await _methodChannel.invokeMethod<bool>('hasPermission');
    return result ?? false;
  }

  Future<bool> requestPermission() async {
    final result =
        await _methodChannel.invokeMethod<bool>('requestPermission');
    return result ?? false;
  }

  Future<bool> startRecording({
    int sampleRate = 44100,
    int maxDurationSeconds = 30,
  }) async {
    final result = await _methodChannel.invokeMethod<bool>('startRecording', {
      'sampleRate': sampleRate,
      'maxDurationSeconds': maxDurationSeconds,
    });
    return result ?? false;
  }

  Future<bool> stopRecording() async {
    final result = await _methodChannel.invokeMethod<bool>('stopRecording');
    return result ?? false;
  }

  Stream<List<double>> get chunkStream {
    _chunkStream ??= _eventChannel
        .receiveBroadcastStream()
        .map((event) => (event as List<dynamic>).cast<double>());
    return _chunkStream!;
  }
}
