import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

import 'pulse_x_brand_bindings.dart';

typedef WatermarkState = ({
  bool enabled,
  double witchFlightHeight,
  double witchGlowIntensity,
  int witchStaffSeed,
});

typedef FinalVideoFrame = ({
  int width,
  int height,
  int fps,
  int durationMs,
  bool hasWatermark,
});

/// Single Dart-side gateway to the Paper 7 brand/watermark state
/// API. Routes exclusively through PULSE-X CORE's exported C API.
class PulseXBrand {
  PulseXBrand._(this._bindings);

  static PulseXBrand? _instance;
  final PulseXBrandBindings _bindings;

  factory PulseXBrand() {
    final existing = _instance;
    if (existing != null) return existing;
    final created = PulseXBrand._(PulseXBrandBindings(_openLibrary()));
    _instance = created;
    return created;
  }

  static DynamicLibrary _openLibrary() {
    if (Platform.isAndroid) {
      return DynamicLibrary.open('libpulse_x_core.so');
    }
    throw UnsupportedError('PULSE-X BRAND + VIDEO OUTPUT targets Android only.');
  }

  bool applyWatermark() {
    return _bindings.applyWatermark() == 0;
  }

  WatermarkState? getWatermarkState() {
    final ptr = malloc<PxBrandWatermarkDescriptor>();
    try {
      if (_bindings.getWatermarkState(ptr) != 0) return null;
      return (
        enabled: ptr.ref.enabled != 0,
        witchFlightHeight: ptr.ref.witchFlightHeight,
        witchGlowIntensity: ptr.ref.witchGlowIntensity,
        witchStaffSeed: ptr.ref.witchStaffSeed,
      );
    } finally {
      malloc.free(ptr);
    }
  }

  FinalVideoFrame? getFinalVideoFrame() {
    final ptr = malloc<PxFinalVideoFrame>();
    try {
      if (_bindings.getFinalVideoFrame(ptr) != 0) return null;
      return (
        width: ptr.ref.width,
        height: ptr.ref.height,
        fps: ptr.ref.fps,
        durationMs: ptr.ref.durationMs,
        hasWatermark: ptr.ref.hasWatermark != 0,
      );
    } finally {
      malloc.free(ptr);
    }
  }
}
