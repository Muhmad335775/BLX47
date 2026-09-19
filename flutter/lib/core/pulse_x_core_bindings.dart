import 'dart:ffi';
import 'dart:io';

typedef _InitNative = Int32 Function();
typedef _InitDart = int Function();

typedef _TickNative = Int32 Function(Float dtSeconds);
typedef _TickDart = int Function(double dtSeconds);

class PulseXCore {
  late final DynamicLibrary _lib;
  late final _InitDart pxCoreInit;
  late final _TickDart pxCoreTick;

  PulseXCore() {
    _lib = Platform.isAndroid
        ? DynamicLibrary.open('libpulse_x_core.so')
        : DynamicLibrary.process();

    pxCoreInit = _lib
        .lookup<NativeFunction<_InitNative>>('px_core_init')
        .asFunction();

    pxCoreTick = _lib
        .lookup<NativeFunction<_TickNative>>('px_core_tick')
        .asFunction();
  }
}
