import 'dart:ffi';

typedef PxCoreVersionNative = Int32 Function();
typedef PxCoreVersionDart = int Function();

typedef PxCoreInitNative = Int32 Function();
typedef PxCoreInitDart = int Function();

typedef PxCoreShutdownNative = Void Function();
typedef PxCoreShutdownDart = void Function();

typedef PxCoreFeedTestAudioNative = Int32 Function(
  Pointer<Float> samples,
  Int32 sampleCount,
);
typedef PxCoreFeedTestAudioDart = int Function(
  Pointer<Float> samples,
  int sampleCount,
);

class PulseXCoreBindings {
  PulseXCoreBindings(DynamicLibrary lib)
      : version = lib
            .lookup<NativeFunction<PxCoreVersionNative>>('px_core_version')
            .asFunction(),
        init = lib
            .lookup<NativeFunction<PxCoreInitNative>>('px_core_init')
            .asFunction(),
        shutdown = lib
            .lookup<NativeFunction<PxCoreShutdownNative>>('px_core_shutdown')
            .asFunction(),
        feedTestAudio = lib
            .lookup<NativeFunction<PxCoreFeedTestAudioNative>>(
                'px_core_feed_test_audio')
            .asFunction();

  final PxCoreVersionDart version;
  final PxCoreInitDart init;
  final PxCoreShutdownDart shutdown;
  final PxCoreFeedTestAudioDart feedTestAudio;
}
