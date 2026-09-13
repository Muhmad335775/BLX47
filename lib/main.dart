import 'package:flutter/material.dart';

import 'audio/recording_controller.dart';

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
    });
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
      appBar: AppBar(title: const Text('BLX47 - Paper 3')),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.symmetric(horizontal: 16),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                _statusText,
                textAlign: TextAlign.center,
                style: Theme.of(context).textTheme.bodyMedium,
              ),
              const SizedBox(height: 32),
              GestureDetector(
                behavior: HitTestBehavior.opaque,
                onTapDown: (_) => _controller.onPressStart(),
                onTapUp: (_) => _controller.onPressEnd(),
                onTapCancel: () => _controller.onPressEnd(),
                child: AnimatedBuilder(
                  animation: _pulseController,
                  builder: (context, child) {
                    final scale = isRecording
                        ? 1.0 + (_pulseController.value * 0.3)
                        : 1.0;
                    return SizedBox(
                      width: 120,
                      height: 120,
                      child: Stack(
                        alignment: Alignment.center,
                        children: [
                          if (isRecording)
                            Transform.scale(
                              scale: scale,
                              child: Container(
                                width: 96,
                                height: 96,
                                decoration: BoxDecoration(
                                  shape: BoxShape.circle,
                                  color: Colors.redAccent.withOpacity(0.3),
                                ),
                              ),
                            ),
                          Container(
                            width: 72,
                            height: 72,
                            decoration: BoxDecoration(
                              shape: BoxShape.circle,
                              color: isRecording
                                  ? Colors.redAccent
                                  : Colors.blueGrey,
                            ),
                            child: const Icon(Icons.mic,
                                color: Colors.white, size: 32),
                          ),
                        ],
                      ),
                    );
                  },
                ),
              ),
              if (_phase == RecordingPhase.permissionDenied) ...[
                const SizedBox(height: 24),
                ElevatedButton(
                  onPressed: () => _controller.retryPermission(),
                  child: const Text('Retry'),
                ),
              ],
            ],
          ),
        ),
      ),
    );
  }
}
