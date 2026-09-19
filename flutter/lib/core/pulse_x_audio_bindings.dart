import 'dart:ffi';
import 'dart:io';

typedef _InitNative = Int32 Function(Int32 sampleRate, Int32 bufferFrames);
typedef _InitDart = int Function(int sampleRate, int bufferFrames);

typedef _SetIntensityNative = Int32 Function(Float intensity);
typedef _SetIntensityDart = int Function(double intensity);

class PulseXAudio {
  late final DynamicLibrary _lib;
  late final _InitDart pxAudioInit;
  late final _SetIntensityDart pxAudioSetIntensity;

  PulseXAudio() {
    _lib = Platform.isAndroid
        ? DynamicLibrary.open('libpulse_x_audio.so')
        : DynamicLibrary.process();

    pxAudioInit = _lib
        .lookup<NativeFunction<_InitNative>>('px_audio_init')
        .asFunction();

    pxAudioSetIntensity = _lib
        .lookup<NativeFunction<_SetIntensityNative>>('px_audio_set_intensity')
        .asFunction();
  }
}
