# Slice 009: Voice Output Interface

The typed note boundary made the instrument easier to drive from another
component. The next small step is to separate "what the instrument wants to
sound" from "how this board makes sound".

This slice introduces `VoiceOutput`, a hardware-neutral interface with three
operations: start a MIDI note with a waveform, stop the current note, and report
whether the voice is playing. The interface intentionally knows nothing about
buttons, display rendering, BLE, or receiver state.

`SpeakerToneOutput` now implements `VoiceOutput`. It remains the only class that
knows how to translate the selected waveform into the M5Unified speaker API.
Frequency conversion still uses the existing MIDI note helper, so the public
output contract can stay in MIDI note numbers.

The firmware behavior should stay unchanged. Button A still simulates a C4
Note On while pressed and a Note Off when released. Button B still switches
between `saw32` and `sq32`.

A small native test uses a fake `VoiceOutput` implementation. This does not test
the M5 speaker hardware, but it proves the interface can be implemented and used
without pulling embedded dependencies into pure code.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: native tests pass, firmware builds, and the hardware behavior
matches the previous slice.
