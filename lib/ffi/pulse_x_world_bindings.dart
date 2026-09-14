import 'dart:ffi';

final class PxFireworksParams extends Struct {
  @Float()
  external double launchX;

  @Float()
  external double launchY;

  @Float()
  external double burstRadius;

  @Float()
  external double burstHeight;

  @Int32()
  external int particleCount;

  @Int32()
  external int colorSeed;

  @Int32()
  external int timingOffsetMs;

  @Int32()
  external int reserved;
}

final class PxWorldDescriptor extends Struct {
  @Int32()
  external int elementFlags;

  @Int32()
  external int landmarkId;

  @Int32()
  external int hasFireworks;

  @Int32()
  external int variationSeed;

  external PxFireworksParams fireworks;

  @Float()
  external double atmosphereIntensity;

  @Float()
  external double lightingWarmth;

  @Array(4)
  external Array<Int32> reserved;
}

typedef PxCoreGenerateWorldNative = Int32 Function(Int32 variationSeed);
typedef PxCoreGenerateWorldDart = int Function(int variationSeed);

typedef PxCoreGetWorldDescriptorNative = Int32 Function(
  Pointer<PxWorldDescriptor> outWorld,
);
typedef PxCoreGetWorldDescriptorDart = int Function(
  Pointer<PxWorldDescriptor> outWorld,
);

typedef PxCoreGetDailyLandmarkNative = Int32 Function(Int32 daySeed);
typedef PxCoreGetDailyLandmarkDart = int Function(int daySeed);

class PulseXWorldBindings {
  PulseXWorldBindings(DynamicLibrary lib)
      : generateWorld = lib
            .lookup<NativeFunction<PxCoreGenerateWorldNative>>(
                'px_core_generate_world')
            .asFunction(),
        getWorldDescriptor = lib
            .lookup<NativeFunction<PxCoreGetWorldDescriptorNative>>(
                'px_core_get_world_descriptor')
            .asFunction(),
        getDailyLandmark = lib
            .lookup<NativeFunction<PxCoreGetDailyLandmarkNative>>(
                'px_core_get_daily_landmark')
            .asFunction();

  final PxCoreGenerateWorldDart generateWorld;
  final PxCoreGetWorldDescriptorDart getWorldDescriptor;
  final PxCoreGetDailyLandmarkDart getDailyLandmark;
}
