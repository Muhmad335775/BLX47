import 'dart:async';
import 'package:flutter/services.dart';

import 'audio_capture.dart';
import '../ffi/pulse_x_core.dart';

enum RecordingPhase {
  idle,
  permissionDenied,
  recording,
  processing,
  ready,
  error,
}

class RecordingController {
  RecordingController({
    AudioCapture? audioCapture,
    PulseXCore? core,
  })  : _audio = audioCapture ?? AudioCapture.instance,
        _core = core ?? PulseXCore();

  final AudioCapture _audio;
  final PulseXCore _core;

  StreamSubscription<List<double>>? _chunkSub;
  Timer? _hardLimitTimer;

  final _phaseController = StreamController<RecordingPhase>.broadcast();
  Stream<RecordingPhase> get phaseStream => _phaseController.stream;

  ({double rms, double peak, int sampleCount})? lastAnalysis;
  String? lastErrorMessage;

  static const int _sampleRate = 44100;
  static const int _maxDurationSeconds = 30;

  bool _coreReady = false;

  Future<void> ensureCoreReady() async {
    if (_coreReady) return;
    _coreReady = _core.init(sampleRate: _sampleRate);
  }

  Future<void> onPressStart() async {
    try {
      await ensureCoreReady();

      final hasPermission = await _audio.hasPermission();
      if (!hasPermission) {
        final granted = await _audio.requestPermission();
        if (!granted) {
          _phaseController.add(RecordingPhase.permissionDenied);
          return;
        }
      }

      if (!_core.beginRecording()) {
        lastErrorMessage = 'core.beginRecording() returned false';
        _phaseController.add(RecordingPhase.error);
        return;
      }

      final started = await _audio.startRecording(
        sampleRate: _sampleRate,
        maxDurationSeconds: _maxDurationSeconds,
      );
      if (!started) {
        lastErrorMessage = 'audio.startRecording() returned false';
        _phaseController.add(RecordingPhase.error);
        return;
      }

      HapticFeedback.mediumImpact();
      _phaseController.add(RecordingPhase.recording);

      _chunkSub = _audio.chunkStream.listen((chunk) {
        _core.feedAudio(chunk);
      });

      _hardLimitTimer = Timer(
        const Duration(seconds: _maxDurationSeconds),
        onPressEnd,
      );
    } catch (e, st) {
      lastErrorMessage = 'onPressStart exception: $e\n$st';
      _phaseController.add(RecordingPhase.error);
    }
  }

  Future<void> onPressEnd() async {
    try {
      _hardLimitTimer?.cancel();
      _hardLimitTimer = null;

      if (_phaseController.isClosed) return;
      await _chunkSub?.cancel();
      _chunkSub = null;

      await _audio.stopRecording();
      HapticFeedback.lightImpact();

      _phaseController.add(RecordingPhase.processing);

      final endedOk = _core.endRecording();
      final analysis = _core.getAudioAnalysis();
      lastAnalysis = analysis;

      if (!endedOk || analysis == null) {
        lastErrorMessage =
            'endRecording=$endedOk analysis=${analysis == null ? "null" : "ok"}';
        _phaseController.add(RecordingPhase.error);
        return;
      }

      _phaseController.add(RecordingPhase.ready);
    } catch (e, st) {
      lastErrorMessage = 'onPressEnd exception: $e\n$st';
      _phaseController.add(RecordingPhase.error);
    }
  }

  void retryPermission() {
    _phaseController.add(RecordingPhase.idle);
  }

  void dispose() {
    _hardLimitTimer?.cancel();
    _chunkSub?.cancel();
    _phaseController.close();
  }
}
