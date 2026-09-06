# Slice 008: Typed Note Event Boundary

The last-note priority policy made the instrument robust enough to receive real
note traffic, but callers still had to choose between `noteOn()` and
`noteOff()` methods directly. This slice adds a small typed boundary for note
events.

`NoteEvent` contains only note facts: event type, channel, note, and velocity.
It intentionally has no BLE, parser, display, or speaker details. Pitch bend
and control change are also intentionally outside this type; they should become
separate event contracts when the instrument has a concrete use for them.

`MonophonicInstrument::handleNoteEvent()` translates a `NoteEvent` into the
existing monophonic policy and returns a `VoiceAction`. The channel field is
preserved in the event contract but ignored by the current instrument policy.
That keeps the receiver-to-instrument boundary honest without introducing
channel filtering before we need it.

The Button A hardware smoke test now constructs typed events: pressing A sends
Note On C4, releasing A sends Note Off C4. The observed hardware behavior should
remain unchanged.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: native tests cover typed Note On, typed Note Off, Note On with
velocity zero, and the current channel-ignored policy.
