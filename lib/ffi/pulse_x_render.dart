import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

import 'pulse_x_render_bindings.dart';
import 'pulse_x_voice_scene.dart';

enum PxRenderBackend {
  vulkan(0),
  openglEs(1);

  const PxRenderBackend(this.value);
  final int value;

  static PxRenderBackend fromValue(int value) {
    return value == 1 ? PxRenderBackend.openglEs : PxRenderBackend.vulkan;
  }
}

typedef CinematicRender = ({
  PxRenderBackend backend,
  int fxFlags,
  int particleCount,
  int targetFps,
  bool isLargeScene,
  double atmosphereDensity,
  double cameraFovDeg,
});

/// Single Dart-side gateway to the Paper 6 cinematic renderer API.
/// Routes exclusively through PULSE-X CORE's exported C API -
/// this only reads the composed render descriptor; the actual
/// Vulkan/OpenGL ES draw calls happen in native rendering code
/// that consumes this same descriptor.
class PulseXRender {
  PulseXRender._(this._bindings);

  static PulseXRender? _instance;
  final PulseXRenderBindings _bindings;

  factory PulseXRender() {
    final existing = _instance;
    if (existing != null) return existing;
    final created = PulseXRender._(PulseXRenderBindings(_openLibrary()));
    _instance = created;
    return created;
  }

  static DynamicLibrary _openLibrary() {
    if (Platform.isAndroid) {
      return DynamicLibrary.open('libpulse_x_core.so');
    }
    throw UnsupportedError('PULSE-X CINEMATIC RENDERER targets Android only.');
  }

  bool buildCinematicRender({PxDeviceTier deviceTier = PxDeviceTier.mid}) {
    return _bindings.buildCinematicRender(deviceTier.value) == 0;
  }

  CinematicRender? getCinematicRender() {
    final ptr = malloc<PxCinematicRenderDescriptor>();
    try {
      if (_bindings.getCinematicRender(ptr) != 0) return null;
      return (
        backend: PxRenderBackend.fromValue(ptr.ref.backend),
        fxFlags: ptr.ref.fxFlags,
        particleCount: ptr.ref.particleCount,
        targetFps: ptr.ref.targetFps,
        isLargeScene: ptr.ref.isLargeScene != 0,
        atmosphereDensity: ptr.ref.atmosphereDensity,
        cameraFovDeg: ptr.ref.camera.fieldOfViewDeg,
      );
    } finally {
      malloc.free(ptr);
    }
  }
}
