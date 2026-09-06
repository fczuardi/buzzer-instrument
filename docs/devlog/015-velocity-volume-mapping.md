# Slice 015: Velocity to Volume Mapping

The volume probe showed that the M5StickC Plus2 buzzer has a useful loudness
range, but only within limits. Volume `32` was too weak to recognize reliably,
`64..128` sounded useful, and `192..255` became distorted.

This slice maps MIDI Note On velocity to the constrained M5 speaker volume range
`64..128`. The mapping lives in `SpeakerToneOutput` because that range is
specific to this hardware/backend. The instrument policy does not know about M5
speaker volume.

`VoiceAction` now carries velocity, and `VoiceOutput::startNote()` receives it.
`MonophonicInstrument` stores velocity with each held note so last-note priority
can restore both the previous note and its original velocity when the current
note is released.

The local firmware smoke test now probes velocity instead of raw volume. Button
A plays C4 using the selected velocity, and Button B cycles:

```text
1, 32, 64, 96, 127
```

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Hardware validation should confirm whether the mapped levels feel expressive
without falling into the distorted volume range.
