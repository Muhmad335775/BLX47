import 'package:flutter/material.dart';

import 'audio/recording_controller.dart';
import 'ffi/pulse_x_world.dart';
import 'ffi/pulse_x_voice_scene.dart';
import 'ffi/pulse_x_render.dart';

void main() {
  runApp(const Blx47App());
}

class Blx47App extends StatelessWidget {
  const Blx47App({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'BLX47',
      theme: ThemeData.dark(useMaterial3: true),
      home: const RecordingPage(),
    );
  }
}

class RecordingPage extends StatefulWidget {
  const RecordingPage({super.key});

  @override
  State<RecordingPage> createState() => _RecordingPageState();
}

class _RecordingPageState extends State<RecordingPage>
    with SingleTickerProviderStateMixin {
  final RecordingController _controller = RecordingController();
  late final AnimationController _pulseController;

  RecordingPhase _phase = RecordingPhase.idle;
  String? _worldDebugText;
  String? _sceneDebugText;
  String? _renderDebugText;

  @override
  void initState() {
    super.initState();
    _pulseController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 900),
    )..repeat(reverse: true);

    _controller.phaseStream.listen((phase) {
      if (!mounted) return;
      setState(() => _phase = phase);
      if (phase == RecordingPhase.ready) {
        _runGenerationChecks();
      }
    });
  }

  void _runGenerationChecks() {
    _runWorldCheck();
    _runVoiceSceneCheck();
    _runRenderCheck();
    _controller.returnCoreToReady();
  }

  void _runWorldCheck() {
    try {
      final world = PulseXWorld();
      final generated = world.generateWorld(
        variationSeed: DateTime.now().millisecondsSinceEpoch,
      );
      final descriptor = world.getWorldDescriptor();
      final landmark = world.getDailyLandmark(daySeed: DateTime.now().day);

      if (!generated || descriptor == null) {
        setState(() {
          _worldDebugText =
              'WORLD ERROR: generated=$generated descriptor=$descriptor';
        });
        return;
      }

      setState(() {
        _worldDebugText =
            'World OK - flags: ${descriptor.elementFlags} - '
            'fireworks: ${descriptor.hasFireworks} - '
            'atmosphere: ${descriptor.atmosphereIntensity.toStringAsFixed(2)} - '
            'dailyLandmark: ${landmark.name}';
      });
    } catch (e, st) {
      setState(() {
        _worldDebugText = 'WORLD EXCEPTION: $e\n$st';
      });
    }
  }

  void _runVoiceSceneCheck() {
    try {
      final voiceScene = PulseXVoiceScene();
      final generated = voiceScene.generateVoiceScene(
        deviceTier: PxDeviceTier.high,
      );
      final scene = voiceScene.getVoiceScene();

      if (!generated || scene == null) {
        setState(() {
          _sceneDebugText =
              'SCENE ERROR: generated=$generated scene=$scene';
        });
        return;
      }

      final variationsText =
          scene.variations.map((f) => f.name).join(', ');

      setState(() {
        _sceneDebugText =
            'Scene OK - family: ${scene.family.name} - '
            'mood: ${scene.moodIntensity.toStringAsFixed(2)} - '
            'movement: ${scene.mapping.movement.toStringAsFixed(2)} - '
            'visual: ${scene.mapping.visualResponse.toStringAsFixed(2)} - '
            'timing: ${scene.mapping.animationTiming.toStringAsFixed(2)} - '
            'variations: [$variationsText]';
      });
    } catch (e, st) {
      setState(() {
        _sceneDebugText = 'SCENE EXCEPTION: $e\n$st';
      });
    }
  }

  void _runRenderCheck() {
    try {
      final render = PulseXRender();
      final built = render.buildCinematicRender(
        deviceTier: PxDeviceTier.high,
      );
      final result = render.getCinematicRender();

      if (!built || result == null) {
        setState(() {
          _renderDebugText = 'RENDER ERROR: built=$built result=$result';
        });
        return;
      }

      setState(() {
        _renderDebugText =
            'Render OK - backend: ${result.backend.name} - '
            'fx: ${result.fxFlags} - '
            'particles: ${result.particleCount} - '
            'fps: ${result.targetFps} - '
            'largeScene: ${result.isLargeScene} - '
            'atmosphere: ${result.atmosphereDensity.toStringAsFixed(2)} - '
            'fov: ${result.cameraFovDeg.toStringAsFixed(1)}';
      });
    } catch (e, st) {
      setState(() {
        _renderDebugText = 'RENDER EXCEPTION: $e\n$st';
      });
    }
  }

  @override
  void dispose() {
    _pulseController.dispose();
    _controller.dispose();
    super.dispose();
  }

  String get _statusText {
    switch (_phase) {
      case RecordingPhase.idle:
        return 'Press and hold to record';
      case RecordingPhase.permissionDenied:
        return 'Microphone permission is needed to record.';
      case RecordingPhase.recording:
        return 'Recording...';
      case RecordingPhase.processing:
        return 'Processing...';
      case RecordingPhase.ready:
        final a = _controller.lastAnalysis;
        if (a == null) return 'Done';
        return 'RMS ${a.rms.toStringAsFixed(4)} - Peak ${a.peak.toStringAsFixed(4)} - Samples ${a.sampleCount}';
      case RecordingPhase.error:
        return 'ERROR: ${_controller.lastErrorMessage ?? "unknown"}';
    }
  }

  @override
  Widget build(BuildContext context) {
    final isRecording = _phase == RecordingPhase.recording;

    return Scaffold(
      appBar: AppBar(title: const Text('BLX47 - Paper 6')),
      body: Stack(
        children: [
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 24),
            child: Align(
              alignment: Alignment.topCenter,
              child: Column(
                children: [
                  Text(
                    _statusText,
                    textAlign: TextAlign.center,
                    style: Theme.of(context).textTheme.bodyMedium,
                  ),
                  if (_worldDebugText != null) ...[
                    const SizedBox(height: 12),
                    Text(
                      _worldDebugText!,
                      textAlign: TextAlign.center,
                      style: Theme.of(context).textTheme.bodySmall,
                    ),
                  ],
                  if (_sceneDebugText != null) ...[
                    const SizedBox(height: 12),
                    Text(
                      _sceneDebugText!,
                      textAlign: TextAlign.center,
                      style: Theme.of(context).textTheme.bodySmall,
                    ),
                  ],
                  if (_renderDebugText != null) ...[
                    const SizedBox(height: 12),
                    Text(
                      _renderDebugText!,
                      textAlign: TextAlign.center,
                      style: Theme.of(context).textTheme.bodySmall,
                    ),
                  ],
                ],
              ),
            ),
          ),
          if (_phase == RecordingPhase.permissionDenied)
            Center(
              child: ElevatedButton(
                onPressed: () => _controller.retryPermission(),
                child: const Text('Retry'),
              ),
            ),
          Positioned(
            right: 20,
            bottom: 28,
            child: GestureDetector(
              behavior: HitTestBehavior.opaque,
              onTapDown: (_) => _controller.onPressStart(),
              onTapUp: (_) => _controller.onPressEnd(),
              onTapCancel: () => _controller.onPressEnd(),
              child: AnimatedBuilder(
                animation: _pulseController,
                builder: (context, child) {
                  final scale =
                      isRecording ? 1.0 + (_pulseController.value * 0.3) : 1.0;
                  return SizedBox(
                    width: 96,
                    height: 96,
                    child: Stack(
                      alignment: Alignment.center,
                      children: [
                        if (isRecording)
                          Transform.scale(
                            scale: scale,
                            child: Container(
                              width: 80,
                              height: 80,
                              decoration: BoxDecoration(
                                shape: BoxShape.circle,
                                color: Colors.redAccent.withOpacity(0.3),
                              ),
                            ),
                          ),
                        Container(
                          width: 60,
                          height: 60,
                          decoration: BoxDecoration(
                            shape: BoxShape.circle,
                            color: isRecording
                                ? Colors.redAccent
                                : Colors.blueGrey,
                            boxShadow: const [
                              BoxShadow(
                                color: Colors.black45,
                                blurRadius: 8,
                                offset: Offset(0, 3),
                              ),
                            ],
                          ),
                          child: const Icon(Icons.mic,
                              color: Colors.white, size: 28),
                        ),
                      ],
                    ),
                  );
                },
              ),
            ),
          ),
        ],
      ),
    );
  }
}
