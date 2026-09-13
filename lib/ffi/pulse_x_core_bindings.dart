import 'dart:ffi';

final class PxEngineConfig extends Struct {
  @Int32()
  external int sampleRate;

  @Int32()
  external int maxRecordingSeconds;

  @Int32()
  external int renderWidth;

  @Int32()
  external int renderHeight;

  @Array(4)
  external Array<Int32> reserved;
}

final class PxAudioAnalysis extends Struct {
  @Float()
  external double rmsLevel;

  @Float()
  external double peakLevel;

  @Float()
  external double dominantFrequencyHz;

  @Int32()
  external int sampleCount;
}

final class PxSceneDescriptor extends Struct {
  @Float()
  external double energy;

  @Float()
  external double tempoBpm;

  @Int32()
  external int sceneId;

  @Array(4)
  external Array<Int32> reserved;
}

final class PxRenderFrame extends Struct {
  @Int32()
  external int width;

  @Int32()
  external int height;

  @Int32()
  external int frameIndex;

  @Int32()
  external int isValid;
}

final class PxExportResult extends Struct {
  @Int32()
  external int success;

  @Int32()
  external int errorCode;

  @Array(256)
  external Array<Uint8> outputPath;
}

final class PxStorageState extends Struct {
  @Int64()
  external int bytesUsed;

  @Int64()
  external int bytesAvailable;

  @Array(2)
  external Array<Int32> reserved;
}

typedef PxCoreVersionNative = Int32 Function();
typedef PxCoreVersionDart = int Function();

typedef PxCoreInitNative = Int32 Function(Pointer<PxEngineConfig> config);
typedef PxCoreInitDart = int Function(Pointer<PxEngineConfig> config);

typedef PxCoreShutdownNative = Void Function();
typedef PxCoreShutdownDart = void Function();

typedef PxCoreGetStateNative = Int32 Function();
typedef PxCoreGetStateDart = int Function();

typedef PxCoreGetLastErrorNative = Int32 Function();
typedef PxCoreGetLastErrorDart = int Function();

typedef PxCoreRecoverNative = Int32 Function();
typedef PxCoreRecoverDart = int Function();

typedef PxCoreBeginRecordingNative = Int32 Function();
typedef PxCoreBeginRecordingDart = int Function();

typedef PxCoreFeedAudioNative = Int32 Function(
  Pointer<Float> samples,
  Int32 sampleCount,
);
typedef PxCoreFeedAudioDart = int Function(
  Pointer<Float> samples,
  int sampleCount,
);

typedef PxCoreEndRecordingNative = Int32 Function();
typedef PxCoreEndRecordingDart = int Function();

typedef PxCoreGetAudioAnalysisNative = Int32 Function(
  Pointer<PxAudioAnalysis> outAnalysis,
);
typedef PxCoreGetAudioAnalysisDart = int Function(
  Pointer<PxAudioAnalysis> outAnalysis,
);

typedef PxCoreGetSceneDescriptorNative = Int32 Function(
  Pointer<PxSceneDescriptor> outScene,
);
typedef PxCoreGetSceneDescriptorDart = int Function(
  Pointer<PxSceneDescriptor> outScene,
);

typedef PxCoreBeginRenderNative = Int32 Function();
typedef PxCoreBeginRenderDart = int Function();

typedef PxCoreGetRenderFrameNative = Int32 Function(
  Pointer<PxRenderFrame> outFrame,
);
typedef PxCoreGetRenderFrameDart = int Function(
  Pointer<PxRenderFrame> outFrame,
);

typedef PxCoreBeginExportNative = Int32 Function();
typedef PxCoreBeginExportDart = int Function();

typedef PxCoreGetExportResultNative = Int32 Function(
  Pointer<PxExportResult> outResult,
);
typedef PxCoreGetExportResultDart = int Function(
  Pointer<PxExportResult> outResult,
);

typedef PxCoreGetStorageStateNative = Int32 Function(
  Pointer<PxStorageState> outStorage,
);
typedef PxCoreGetStorageStateDart = int Function(
  Pointer<PxStorageState> outStorage,
);

typedef PxCoreReturnToReadyNative = Int32 Function();
typedef PxCoreReturnToReadyDart = int Function();

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
        getState = lib
            .lookup<NativeFunction<PxCoreGetStateNative>>('px_core_get_state')
            .asFunction(),
        getLastError = lib
            .lookup<NativeFunction<PxCoreGetLastErrorNative>>(
                'px_core_get_last_error')
            .asFunction(),
        recover = lib
            .lookup<NativeFunction<PxCoreRecoverNative>>('px_core_recover')
            .asFunction(),
        beginRecording = lib
            .lookup<NativeFunction<PxCoreBeginRecordingNative>>(
                'px_core_begin_recording')
            .asFunction(),
        feedAudio = lib
            .lookup<NativeFunction<PxCoreFeedAudioNative>>(
                'px_core_feed_audio')
            .asFunction(),
        endRecording = lib
            .lookup<NativeFunction<PxCoreEndRecordingNative>>(
                'px_core_end_recording')
            .asFunction(),
        getAudioAnalysis = lib
            .lookup<NativeFunction<PxCoreGetAudioAnalysisNative>>(
                'px_core_get_audio_analysis')
            .asFunction(),
        getSceneDescriptor = lib
            .lookup<NativeFunction<PxCoreGetSceneDescriptorNative>>(
                'px_core_get_scene_descriptor')
            .asFunction(),
        beginRender = lib
            .lookup<NativeFunction<PxCoreBeginRenderNative>>(
                'px_core_begin_render')
            .asFunction(),
        getRenderFrame = lib
            .lookup<NativeFunction<PxCoreGetRenderFrameNative>>(
                'px_core_get_render_frame')
            .asFunction(),
        beginExport = lib
            .lookup<NativeFunction<PxCoreBeginExportNative>>(
                'px_core_begin_export')
            .asFunction(),
        getExportResult = lib
            .lookup<NativeFunction<PxCoreGetExportResultNative>>(
                'px_core_get_export_result')
            .asFunction(),
        getStorageState = lib
            .lookup<NativeFunction<PxCoreGetStorageStateNative>>(
                'px_core_get_storage_state')
            .asFunction(),
        returnToReady = lib
            .lookup<NativeFunction<PxCoreReturnToReadyNative>>(
                'px_core_return_to_ready')
            .asFunction();

  final PxCoreVersionDart version;
  final PxCoreInitDart init;
  final PxCoreShutdownDart shutdown;
  final PxCoreGetStateDart getState;
  final PxCoreGetLastErrorDart getLastError;
  final PxCoreRecoverDart recover;
  final PxCoreBeginRecordingDart beginRecording;
  final PxCoreFeedAudioDart feedAudio;
  final PxCoreEndRecordingDart endRecording;
  final PxCoreGetAudioAnalysisDart getAudioAnalysis;
  final PxCoreGetSceneDescriptorDart getSceneDescriptor;
  final PxCoreBeginRenderDart beginRender;
  final PxCoreGetRenderFrameDart getRenderFrame;
  final PxCoreBeginExportDart beginExport;
  final PxCoreGetExportResultDart getExportResult;
  final PxCoreGetStorageStateDart getStorageState;
  final PxCoreReturnToReadyDart returnToReady;
}
