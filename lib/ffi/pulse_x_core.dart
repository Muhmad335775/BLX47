import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

import 'pulse_x_core_bindings.dart';

enum PxState {
  uninitialized(0),
  ready(1),
  recording(2),
  processing(3),
  generation(4),
  render(5),
  export_(6),
  error(7);

  const PxState(this.value);
  final int value;

  static PxState fromValue(int value) {
    return PxState.values.firstWhere(
      (s) => s.value == value,
      orElse: () => PxState.error,
    );
  }
}

/// Single Dart-side gateway to PULSE-X CORE.
/// All other Flutter code must go through this class only -
/// no other file may open the native library or call FFI directly.
class PulseXCore {
  PulseXCore._(this._bindings);

  static PulseXCore? _instance;
  final PulseXCoreBindings _bindings;

  factory PulseXCore() {
    final existing = _instance;
    if (existing != null) return existing;
    final created = PulseXCore._(PulseXCoreBindings(_openLibrary()));
    _instance = created;
    return created;
  }

  static DynamicLibrary _openLibrary() {
    if (Platform.isAndroid) {
      return DynamicLibrary.open('libpulse_x_core.so');
    }
    throw UnsupportedError('PULSE-X CORE targets Android only.');
  }

  int get nativeVersion => _bindings.version();

  PxState get state => PxState.fromValue(_bindings.getState());

  int get lastError => _bindings.getLastError();

  bool init({
    int sampleRate = 44100,
    int maxRecordingSeconds = 60,
    int renderWidth = 1080,
    int renderHeight = 1920,
  }) {
    final configPtr = malloc<PxEngineConfig>();
    try {
      configPtr.ref.sampleRate = sampleRate;
      configPtr.ref.maxRecordingSeconds = maxRecordingSeconds;
      configPtr.ref.renderWidth = renderWidth;
      configPtr.ref.renderHeight = renderHeight;
      return _bindings.init(configPtr) == 0;
    } finally {
      malloc.free(configPtr);
    }
  }

  void shutdown() => _bindings.shutdown();

  bool recover() => _bindings.recover() == 0;

  bool beginRecording() => _bindings.beginRecording() == 0;

  bool feedAudio(List<double> samples) {
    final ptr = malloc<Float>(samples.length);
    try {
      for (var i = 0; i < samples.length; i++) {
        ptr[i] = samples[i];
      }
      return _bindings.feedAudio(ptr, samples.length) == 0;
    } finally {
      malloc.free(ptr);
    }
  }

  bool endRecording() => _bindings.endRecording() == 0;

  ({double rms, double peak, int sampleCount})? getAudioAnalysis() {
    final ptr = malloc<PxAudioAnalysis>();
    try {
      if (_bindings.getAudioAnalysis(ptr) != 0) return null;
      return (
        rms: ptr.ref.rmsLevel,
        peak: ptr.ref.peakLevel,
        sampleCount: ptr.ref.sampleCount,
      );
    } finally {
      malloc.free(ptr);
    }
  }

  bool beginRender() => _bindings.beginRender() == 0;

  bool beginExport() => _bindings.beginExport() == 0;

  bool returnToReady() => _bindings.returnToReady() == 0;
}
