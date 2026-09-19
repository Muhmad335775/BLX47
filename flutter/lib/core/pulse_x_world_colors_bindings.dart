import 'dart:ffi';
import 'dart:io';
import 'package:ffi/ffi.dart';

final class PxLight extends Struct {
  @Float() external double hue;
  @Float() external double saturation;
  @Float() external double value;
  @Int8() external int isKeyLight;
}

typedef _GenerateNative = Int32 Function(
    Uint32 seed, Pointer<PxLight> outLights, Int32 maxLights, Pointer<Int32> outCount);
typedef _GenerateDart = int Function(
    int seed, Pointer<PxLight> outLights, int maxLights, Pointer<Int32> outCount);

class PulseXWorldColors {
  late final DynamicLibrary _lib;
  late final _GenerateDart pxWorldColorsGenerate;

  PulseXWorldColors() {
    _lib = Platform.isAndroid
        ? DynamicLibrary.open('libpulse_x_world_colors.so')
        : DynamicLibrary.process();

    pxWorldColorsGenerate = _lib
        .lookup<NativeFunction<_GenerateNative>>('px_world_colors_generate')
        .asFunction();
  }
}
