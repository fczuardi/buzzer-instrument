# m5-tone-output

PlatformIO package for M5Unified speaker-like `VoiceOutput` backends used by
the embedded music experiments.

Current public classes:

- `M5ToneOutputCore`: shared M5Unified tone playback, waveform, volume, and
  velocity-to-volume behavior.
- `M5BuzzerToneOutput`: M5StickC Plus2 buzzer configuration backed by
  `M5ToneOutputCore`.
- `M5CoreGrayToneOutput`: M5Stack Core Gray internal speaker defaults backed by
  `M5ToneOutputCore`.

Starting with `0.1.3`, the PlatformIO Registry package declares its dependency
on `fcz2/monophonic-instrument`, so registry consumers can install
`m5-tone-output` directly:

```ini
lib_deps =
  fcz2/m5-tone-output@0.1.3
```

The package will also host regular M5 speaker output classes, starting with the
Core Gray speaker backend.
