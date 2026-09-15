import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

import 'pulse_x_scene_bindings.dart';

enum PxScenarioFamily {
  comedy(0),
  mocking(1),
  crazy(2),
  horror(3),
  crying(4),
  rage(5),
  dancing(6),
  street(7),
  travel(8),
  beach(9),
  hospital(10),
  fantasy(11),
  witchSpecial(12);

  const PxScenarioFamily(this.value);
  final int value;

  static PxScenarioFamily fromValue(int value) {
    return PxScenarioFamily.values.firstWhere(
      (f) => f.value == value,
      orElse: () => PxScenarioFamily.comedy,
    );
  }
}

enum PxDeviceTier {
  low(0),
  mid(1),
  high(2);

  const PxDeviceTier(this.value);
  final int value;
}

typedef VoiceMapping = ({
  double movement,
  double visualResponse,
  double animationTiming,
  double effects,
  double scaleMotionLighting,
});

typedef VoiceScene = ({
  PxScenarioFamily family,
  double moodIntensity,
  VoiceMapping mapping,
  bool isLivePreview,
  List<PxScenarioFamily> variations,
});

/// Single Dart-side gateway to the Paper 5 voice scene engine API.
/// Routes exclusively through PULSE-X CORE's exported C API.
class PulseXVoiceScene {
  PulseXVoiceScene._(this._bindings);

  static PulseXVoiceScene? _instance;
  final PulseXSceneBindings _bindings;

  factory PulseXVoiceScene() {
    final existing = _instance;
    if (existing != null) return existing;
    final created = PulseXVoiceScene._(PulseXSceneBindings(_openLibrary()));
    _instance = created;
    return created;
  }

  static DynamicLibrary _openLibrary() {
    if (Platform.isAndroid) {
      return DynamicLibrary.open('libpulse_x_core.so');
    }
    throw UnsupportedError('PULSE-X VOICE SCENE ENGINE targets Android only.');
  }

  VoiceScene? _readDescriptor(Pointer<PxVoiceSceneDescriptor> ptr) {
    final variations = <PxScenarioFamily>[];
    final count = ptr.ref.variationCount.clamp(0, 3);
    for (var i = 0; i < count; i++) {
      variations.add(PxScenarioFamily.fromValue(ptr.ref.variationFamilies[i]));
    }

    return (
      family: PxScenarioFamily.fromValue(ptr.ref.scenarioFamily),
      moodIntensity: ptr.ref.moodIntensity,
      mapping: (
        movement: ptr.ref.mapping.movementAmount,
        visualResponse: ptr.ref.mapping.visualResponse,
        animationTiming: ptr.ref.mapping.animationTimingScale,
        effects: ptr.ref.mapping.effectsIntensity,
        scaleMotionLighting: ptr.ref.mapping.scaleMotionLighting,
      ),
      isLivePreview: ptr.ref.isLivePreview != 0,
      variations: variations,
    );
  }

  /// Call while the user is still holding the record button.
  /// Returns null if CORE isn't currently in RECORDING.
  VoiceScene? getLiveVoiceScene() {
    final ptr = malloc<PxVoiceSceneDescriptor>();
    try {
      if (_bindings.getLiveVoiceScene(ptr) != 0) return null;
      return _readDescriptor(ptr);
    } finally {
      malloc.free(ptr);
    }
  }

  bool generateVoiceScene({PxDeviceTier deviceTier = PxDeviceTier.mid}) {
    return _bindings.generateVoiceScene(deviceTier.value) == 0;
  }

  VoiceScene? getVoiceScene() {
    final ptr = malloc<PxVoiceSceneDescriptor>();
    try {
      if (_bindings.getVoiceScene(ptr) != 0) return null;
      return _readDescriptor(ptr);
    } finally {
      malloc.free(ptr);
    }
  }

  ({double flightHeight, double glowIntensity, int staffSeed})? getWitchSpecial() {
    final ptr = malloc<PxWitchSpecialDescriptor>();
    try {
      if (_bindings.getWitchSpecial(ptr) != 0) return null;
      return (
        flightHeight: ptr.ref.flightHeight,
        glowIntensity: ptr.ref.glowIntensity,
        staffSeed: ptr.ref.staffSeed,
      );
    } finally {
      malloc.free(ptr);
    }
  }
}
