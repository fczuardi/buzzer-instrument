# Slice 010: Instrument Event Sink

The previous slice created `VoiceOutput`, a hardware-neutral way to start and
stop sound. This slice adds the matching input side: `InstrumentEventSink`.

`InstrumentEventSink` is the contract a future event producer should target. It
accepts typed `NoteEvent` values and a disconnection signal. It does not expose
button state, display details, BLE internals, or speaker APIs.

`MonophonicInstrumentSink` is the first implementation. It receives incoming
events, asks `MonophonicInstrument` what should happen, and applies the resulting
voice action to a `VoiceOutput`. This keeps the monophonic priority policy, MIDI
event contract, and hardware output connected without merging their
responsibilities.

The local firmware still uses Button A as a smoke-test event producer. Pressing
A sends Note On C4 to the sink, and releasing A sends Note Off C4. Button B
still toggles the test waveform directly because it is local firmware UI, not a
receiver event yet.

Disconnection now has a typed entry point on the sink. The current button test
does not trigger it, but native tests verify that `onDisconnected()` stops the
voice and clears held notes so a future BLE disconnect cannot leave stale note
state inside the instrument.

`onDisconnected()` also calls `VoiceOutput::stopNote()` unconditionally. This
is intentionally defensive: even if the instrument state and physical output
somehow diverge, disconnection still means the audible output must become
silent.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: native tests pass, firmware builds, and Button A still plays
C4 only while held.
