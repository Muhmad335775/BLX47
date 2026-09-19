import 'dart:ffi';
import 'dart:io';

typedef _InitNative = Int32 Function(Int32 sampleRate, Int32 frameSize);
typedef _InitDart = int Function(int sampleRate, int frameSize);

typedef _SetPermissionNative = Int32 Function(Int8 granted);
typedef _SetPermissionDart = int Function(int granted);

class PulseXVoice {
  late final DynamicLibrary _lib;
  late final _InitDart pxVoiceInit;
  late final _SetPermissionDart pxVoiceSetPermission;

  PulseXVoice() {
    _lib = Platform.isAndroid
        ? DynamicLibrary.open('libpulse_x_voice.so')
        : DynamicLibrary.process();

    pxVoiceInit = _lib
        .lookup<NativeFunction<_InitNative>>('px_voice_init')
        .asFunction();

    pxVoiceSetPermission = _lib
        .lookup<NativeFunction<_SetPermissionNative>>('px_voice_set_permission_granted')
        .asFunction();
  }
}
