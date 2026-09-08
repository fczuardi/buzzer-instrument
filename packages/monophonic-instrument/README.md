# monophonic-instrument

PlatformIO package for the shared monophonic instrument policy used by the
embedded music experiments.

This package owns musical state and contracts between event producers and voice
outputs. It does not know any concrete M5 speaker, buzzer, display, button, or
BLE transport.

Current contents:

- `MidiNote`
- `ToneWaveform`
- `VoiceOutput`
- `MonophonicInstrument`
- `MonophonicInstrumentSink`
- `ToneWaveformSamples`

Local sibling projects can consume it with:

```ini
lib_deps =
  file://../../monophonic-instrument/packages/monophonic-instrument
```

Remote distribution is intentionally not settled yet. The migration plan is to
prove a versioned package archive strategy before moving every backend.
