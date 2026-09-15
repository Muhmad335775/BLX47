import 'dart:ffi';

final class PxVoiceMapping extends Struct {
  @Float()
  external double movementAmount;

  @Float()
  external double visualResponse;

  @Float()
  external double animationTimingScale;

  @Float()
  external double effectsIntensity;

  @Float()
  external double scaleMotionLighting;
}

final class PxVoiceSceneDescriptor extends Struct {
  @Int32()
  external int scenarioFamily;

  @Float()
  external double moodIntensity;

  external PxVoiceMapping mapping;

  @Int32()
  external int isLivePreview;

  @Int32()
  external int deviceTier;

  @Int32()
  external int variationCount;

  @Array(3)
  external Array<Int32> variationFamilies;

  @Array(4)
  external Array<Int32> reserved;
}

final class PxWitchSpecialDescriptor extends Struct {
  @Float()
  external double flightHeight;

  @Float()
  external double glowIntensity;

  @Int32()
  external int staffSeed;

  @Array(3)
  external Array<Int32> reserved;
}

typedef PxCoreGetLiveVoiceSceneNative = Int32 Function(
  Pointer<PxVoiceSceneDescriptor> outScene,
);
typedef PxCoreGetLiveVoiceSceneDart = int Function(
  Pointer<PxVoiceSceneDescriptor> outScene,
);

typedef PxCoreGenerateVoiceSceneNative = Int32 Function(Int32 deviceTier);
typedef PxCoreGenerateVoiceSceneDart = int Function(int deviceTier);

typedef PxCoreGetVoiceSceneNative = Int32 Function(
  Pointer<PxVoiceSceneDescriptor> outScene,
);
typedef PxCoreGetVoiceSceneDart = int Function(
  Pointer<PxVoiceSceneDescriptor> outScene,
);

typedef PxCoreGetWitchSpecialNative = Int32 Function(
  Pointer<PxWitchSpecialDescriptor> outWitch,
);
typedef PxCoreGetWitchSpecialDart = int Function(
  Pointer<PxWitchSpecialDescriptor> outWitch,
);

class PulseXSceneBindings {
  PulseXSceneBindings(DynamicLibrary lib)
      : getLiveVoiceScene = lib
            .lookup<NativeFunction<PxCoreGetLiveVoiceSceneNative>>(
                'px_core_get_live_voice_scene')
            .asFunction(),
        generateVoiceScene = lib
            .lookup<NativeFunction<PxCoreGenerateVoiceSceneNative>>(
                'px_core_generate_voice_scene')
            .asFunction(),
        getVoiceScene = lib
            .lookup<NativeFunction<PxCoreGetVoiceSceneNative>>(
                'px_core_get_voice_scene')
            .asFunction(),
        getWitchSpecial = lib
            .lookup<NativeFunction<PxCoreGetWitchSpecialNative>>(
                'px_core_get_witch_special')
            .asFunction();

  final PxCoreGetLiveVoiceSceneDart getLiveVoiceScene;
  final PxCoreGenerateVoiceSceneDart generateVoiceScene;
  final PxCoreGetVoiceSceneDart getVoiceScene;
  final PxCoreGetWitchSpecialDart getWitchSpecial;
}
