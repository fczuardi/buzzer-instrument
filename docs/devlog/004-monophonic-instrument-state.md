# Slice 004: Monophonic Instrument State

The previous slice proved that 32-sample square and saw tables are the useful
waveform options for the current buzzer experiment. This slice keeps the same
hardware behavior but moves musical state out of `main.cpp`.

The new `MonophonicInstrument` module owns the selected MIDI note, note
duration, and waveform. It is pure C++: no Arduino includes, no display calls,
and no direct access to `M5.Speaker`. That keeps the code easier to read and
gives us a place to test musical behavior before introducing MIDI input.

`main.cpp` is now responsible for M5Stick concerns only: button polling,
display updates, serial logs, and forwarding the selected frequency/waveform to
`SpeakerToneOutput`. `SpeakerToneOutput` remains the hardware backend that
knows how to ask M5Unified to render a sampled tone.

The waveform enum moved into `ToneWaveform` so the instrument state and speaker
backend can share a small vocabulary without making the instrument depend on the
speaker class.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
```

Expected hardware result: behavior is unchanged from the previous slice. Button
A advances one semitone at a time from C3 to C7, Button B toggles between
`saw32` and `sq32`, holding Button A resets to C3, and each selected note plays
for 300 ms.

Follow-up: the C major scale was useful while testing broad pitch ranges, but a
monophonic instrument should expose the full playable keyboard range. The
instrument now stores the selected MIDI note directly and walks chromatically
from C3 to C7 before wrapping back to C3.
