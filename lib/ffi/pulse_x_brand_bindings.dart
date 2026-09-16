import 'dart:ffi';

final class PxBrandWatermarkDescriptor extends Struct {
  @Int32()
  external int enabled;

  @Float()
  external double witchFlightHeight;

  @Float()
  external double witchGlowIntensity;

  @Int32()
  external int witchStaffSeed;

  @Float()
  external double logoAnimationPhase;

  @Int32()
  external int logoPositionCorner;

  @Array(3)
  external Array<Int32> reserved;
}

final class PxFinalVideoFrame extends Struct {
  @Int32()
  external int width;

  @Int32()
  external int height;

  @Int32()
  external int fps;

  @Int32()
  external int durationMs;

  @Int32()
  external int hasWatermark;

  @Array(3)
  external Array<Int32> reserved;
}

typedef PxCoreApplyWatermarkNative = Int32 Function();
typedef PxCoreApplyWatermarkDart = int Function();

typedef PxCoreGetWatermarkStateNative = Int32 Function(
  Pointer<PxBrandWatermarkDescriptor> outWatermark,
);
typedef PxCoreGetWatermarkStateDart = int Function(
  Pointer<PxBrandWatermarkDescriptor> outWatermark,
);

typedef PxCoreGetFinalVideoFrameNative = Int32 Function(
  Pointer<PxFinalVideoFrame> outFrame,
);
typedef PxCoreGetFinalVideoFrameDart = int Function(
  Pointer<PxFinalVideoFrame> outFrame,
);

class PulseXBrandBindings {
  PulseXBrandBindings(DynamicLibrary lib)
      : applyWatermark = lib
            .lookup<NativeFunction<PxCoreApplyWatermarkNative>>(
                'px_core_apply_watermark')
            .asFunction(),
        getWatermarkState = lib
            .lookup<NativeFunction<PxCoreGetWatermarkStateNative>>(
                'px_core_get_watermark_state')
            .asFunction(),
        getFinalVideoFrame = lib
            .lookup<NativeFunction<PxCoreGetFinalVideoFrameNative>>(
                'px_core_get_final_video_frame')
            .asFunction();

  final PxCoreApplyWatermarkDart applyWatermark;
  final PxCoreGetWatermarkStateDart getWatermarkState;
  final PxCoreGetFinalVideoFrameDart getFinalVideoFrame;
}
