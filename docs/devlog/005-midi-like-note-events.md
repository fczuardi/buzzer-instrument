# Slice 005: MIDI-Like Note Events

The previous slice extracted `MonophonicInstrument` from `main.cpp`, but it
still behaved like a local note browser. Button A advanced through notes, which
was useful for debugging pitch ranges but not representative of the receiver we
want to build.

This slice changes the instrument boundary to MIDI-like note events.
`MonophonicInstrument` now exposes `noteOn(midiNote, velocity)`,
`noteOff(midiNote)`, and `stopAll()`. It tracks one active note at a time and
uses a simple monophonic policy: the latest Note On replaces the active note,
and Note Off only stops sound when it matches the active note.

The class also follows the MIDI convention where Note On with velocity `0`
means Note Off. Capturing that behavior here makes it explicit and testable
before real MIDI messages arrive from another module.

The hardware UI is now just an event simulator. Button A sends Note On for C4
while pressed and Note Off for C4 when released. Button B still toggles between
`saw32` and `sq32`. This keeps the hardware test small while moving the code
toward the eventual BLE MIDI receiver integration.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
```

Expected hardware result: pressing Button A starts a C4 tone, releasing Button A
stops it, and Button B changes the waveform used for the next played note.
