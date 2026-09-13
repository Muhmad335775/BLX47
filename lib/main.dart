import 'package:flutter/material.dart';

import 'ffi/pulse_x_core.dart';

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
      home: const FoundationCheckPage(),
    );
  }
}

class FoundationCheckPage extends StatefulWidget {
  const FoundationCheckPage({super.key});

  @override
  State<FoundationCheckPage> createState() => _FoundationCheckPageState();
}

class _FoundationCheckPageState extends State<FoundationCheckPage> {
  String _status = 'Checking PULSE-X CORE...';

  @override
  void initState() {
    super.initState();
    _runFoundationCheck();
  }

  void _runFoundationCheck() {
    try {
      final core = PulseXCore();
      final ok = core.init();
      final version = core.nativeVersion;

      final recordingOk = core.beginRecording();
      final testClip = List<double>.filled(256, 0.0);
      final audioOk = core.feedAudio(testClip);
      final endOk = core.endRecording();
      final analysis = core.getAudioAnalysis();

      setState(() {
        _status = (ok && recordingOk && audioOk && endOk && analysis != null)
            ? 'PULSE-X CORE v$version - build foundation OK'
            : 'PULSE-X CORE failed to initialize';
      });
    } catch (e) {
      setState(() {
        _status = 'PULSE-X CORE error: $e';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('BLX47 - Paper 2')),
      body: Center(
        child: Text(_status, style: Theme.of(context).textTheme.titleMedium),
      ),
    );
  }
}
