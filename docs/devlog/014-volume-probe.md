# Slice 014: Volume Probe

Velocity is part of every MIDI Note On event, but the buzzer should not pretend
to be a normal loudspeaker before the hardware proves that volume control is
musically useful.

This slice adds explicit volume control to `SpeakerToneOutput` and changes the
local firmware smoke test into a volume probe. Button A still plays C4 while
held. Button B cycles through fixed `M5.Speaker` volume levels:

```text
32, 64, 96, 128, 192, 255
```

The reusable instrument policy still ignores velocity. This is intentional. The
goal is to test whether the audio backend can produce useful loudness changes
before mapping MIDI velocity to volume in the main instrument path.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Hardware validation should compare whether the volume steps are audible,
musically useful, or mostly distortion/noise across the currently selected
waveform.

Hardware result on the M5StickC Plus2:

```text
32  - tone is not recognizable
64  - tone is recognizable
96  - recognizable and louder
128 - recognizable and even louder
192 - distorted
255 - more distorted
```

This suggests that volume can be a useful destination for MIDI velocity, but
only in a constrained range. A first velocity mapping should probably stay
between 64 and 128 instead of using the full `0..255` M5 speaker volume range.
