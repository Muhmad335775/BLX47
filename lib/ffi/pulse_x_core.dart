import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

import 'pulse_x_core_bindings.dart';

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
    throw UnsupportedError('PULSE-X CORE (Paper 1) targets Android only.');
  }

  int get nativeVersion => _bindings.version();

  bool init() => _bindings.init() == 0;

  void shutdown() => _bindings.shutdown();

  /// Feeds a short audio sample into CORE to satisfy the
  /// audio-reactivity sanity check required by the Validation Gate.
  bool feedTestAudio(List<double> samples) {
    final Pointer<Float> ptr = malloc<Float>(samples.length);
    try {
      for (var i = 0; i < samples.length; i++) {
        ptr[i] = samples[i];
      }
      return _bindings.feedTestAudio(ptr, samples.length) == 0;
    } finally {
      malloc.free(ptr);
    }
  }
}
