import 'dart:ffi';

final class PxDepthLayerParams extends Struct {
  @Float()
  external double parallaxFactor;

  @Float()
  external double blurAmount;

  @Float()
  external double opacity;

  @Int32()
  external int reserved;
}

final class PxCameraParams extends Struct {
  @Float()
  external double positionX;

  @Float()
  external double positionY;

  @Float()
  external double positionZ;

  @Float()
  external double fieldOfViewDeg;

  @Float()
  external double panSpeed;

  @Float()
  external double tiltSpeed;

  @Float()
  external double dollySpeed;
}

final class PxLightingParams extends Struct {
  @Float()
  external double keyLightIntensity;

  @Float()
  external double fillLightIntensity;

  @Float()
  external double rimLightIntensity;

  @Float()
  external double colorTemperatureShift;
}

final class PxCinematicRenderDescriptor extends Struct {
  @Int32()
  external int backend;

  @Int32()
  external int deviceTier;

  @Int32()
  external int fxFlags;

  @Array(5)
  external Array<PxDepthLayerParams> depthLayers;

  external PxCameraParams camera;

  external PxLightingParams lighting;

  @Float()
  external double atmosphereDensity;

  @Int32()
  external int particleCount;

  @Int32()
  external int targetFps;

  @Int32()
  external int isLargeScene;

  @Array(4)
  external Array<Int32> reserved;
}

typedef PxCoreBuildCinematicRenderNative = Int32 Function(Int32 deviceTier);
typedef PxCoreBuildCinematicRenderDart = int Function(int deviceTier);

typedef PxCoreGetCinematicRenderNative = Int32 Function(
  Pointer<PxCinematicRenderDescriptor> outRender,
);
typedef PxCoreGetCinematicRenderDart = int Function(
  Pointer<PxCinematicRenderDescriptor> outRender,
);

class PulseXRenderBindings {
  PulseXRenderBindings(DynamicLibrary lib)
      : buildCinematicRender = lib
            .lookup<NativeFunction<PxCoreBuildCinematicRenderNative>>(
                'px_core_build_cinematic_render')
            .asFunction(),
        getCinematicRender = lib
            .lookup<NativeFunction<PxCoreGetCinematicRenderNative>>(
                'px_core_get_cinematic_render')
            .asFunction();

  final PxCoreBuildCinematicRenderDart buildCinematicRender;
  final PxCoreGetCinematicRenderDart getCinematicRender;
}
