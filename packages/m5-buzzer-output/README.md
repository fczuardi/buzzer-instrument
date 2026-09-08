# m5-buzzer-output

PlatformIO package for the M5StickC Plus2 buzzer `VoiceOutput` backend used by
the embedded music experiments.

This package owns the M5Unified speaker configuration for the Plus2 buzzer,
short wavetable rendering, and velocity-to-volume calibration. It does not own
note priority, pitch bend policy, MIDI transport, display, or buttons.

Current contents:

- `SpeakerToneOutput`
- `VelocityVolumeRange`

Local sibling projects can consume it with:

```ini
lib_deps =
  monophonic-instrument=file://../../monophonic-instrument/packages/monophonic-instrument
  m5-buzzer-output=file://../../monophonic-instrument/packages/m5-buzzer-output
```

The explicit `monophonic-instrument` dependency is required while these packages
are consumed from local paths or archives. Once the shared policy package has a
stable registry or release URL, this package can declare that version directly.

Remote distribution is intentionally not settled yet. The migration plan is to
prove local and archive consumption before switching the hardware showcases.
