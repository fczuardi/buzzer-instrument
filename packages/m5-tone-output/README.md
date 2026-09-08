# m5-tone-output

PlatformIO package for M5Unified speaker-like `VoiceOutput` backends used by
the embedded music experiments.

Current public classes:

- `M5ToneOutputCore`: shared M5Unified tone playback, waveform, volume, and
  velocity-to-volume behavior.
- `M5BuzzerToneOutput`: M5StickC Plus2 buzzer configuration backed by
  `M5ToneOutputCore`.

Consumers must also depend on `monophonic-instrument` while these packages are
consumed from local paths or archives:

```ini
lib_deps =
  monophonic-instrument=file://../../monophonic-instrument/packages/monophonic-instrument
  m5-tone-output=file://../../monophonic-instrument/packages/m5-tone-output
```

The package will also host regular M5 speaker output classes, starting with the
Core Gray speaker backend.
