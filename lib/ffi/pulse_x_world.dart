import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

import 'pulse_x_world_bindings.dart';

enum PxLandmark {
  none(0),
  burjKhalifa(1),
  eiffelTower(2),
  statueOfLiberty(3),
  greatPyramidsGiza(4);

  const PxLandmark(this.value);
  final int value;

  static PxLandmark fromValue(int value) {
    return PxLandmark.values.firstWhere(
      (l) => l.value == value,
      orElse: () => PxLandmark.none,
    );
  }
}

class PxSceneElements {
  static const int sky = 1 << 0;
  static const int moon = 1 << 1;
  static const int stars = 1 << 2;
  static const int sea = 1 << 3;
  static const int birds = 1 << 4;
  static const int butterflies = 1 << 5;
  static const int angels = 1 << 6;
  static const int fantasy = 1 << 7;
  static const int flags = 1 << 8;
  static const int celebration = 1 << 9;
}

/// Single Dart-side gateway to the Paper 4 procedural world API.
/// Routes exclusively through PULSE-X CORE's exported C API -
/// never touches Papers 3-8 internals directly.
class PulseXWorld {
  PulseXWorld._(this._bindings);

  static PulseXWorld? _instance;
  final PulseXWorldBindings _bindings;

  factory PulseXWorld() {
    final existing = _instance;
    if (existing != null) return existing;
    final created = PulseXWorld._(PulseXWorldBindings(_openLibrary()));
    _instance = created;
    return created;
  }

  static DynamicLibrary _openLibrary() {
    if (Platform.isAndroid) {
      return DynamicLibrary.open('libpulse_x_core.so');
    }
    throw UnsupportedError('PULSE-X PROCEDURAL WORLD targets Android only.');
  }

  bool generateWorld({int variationSeed = 0}) {
    return _bindings.generateWorld(variationSeed) == 0;
  }

  ({
    int elementFlags,
    PxLandmark landmark,
    bool hasFireworks,
    double atmosphereIntensity,
    double lightingWarmth,
  })? getWorldDescriptor() {
    final ptr = malloc<PxWorldDescriptor>();
    try {
      if (_bindings.getWorldDescriptor(ptr) != 0) return null;
      return (
        elementFlags: ptr.ref.elementFlags,
        landmark: PxLandmark.fromValue(ptr.ref.landmarkId),
        hasFireworks: ptr.ref.hasFireworks != 0,
        atmosphereIntensity: ptr.ref.atmosphereIntensity,
        lightingWarmth: ptr.ref.lightingWarmth,
      );
    } finally {
      malloc.free(ptr);
    }
  }

  PxLandmark getDailyLandmark({required int daySeed}) {
    final value = _bindings.getDailyLandmark(daySeed);
    return PxLandmark.fromValue(value);
  }
}
