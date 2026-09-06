# Backlog

## Promote `NoteEvent` As The Public Instrument Boundary

`MonophonicInstrument` currently exposes both low-level event methods:

```cpp
VoiceAction noteOn(uint8_t midiNoteNumber, uint8_t velocity);
VoiceAction noteOff(uint8_t midiNoteNumber);
```

and the typed contract method:

```cpp
VoiceAction handleNoteEvent(const NoteEvent& event);
```

This is useful while the note-event contract is still experimental: tests can
exercise the core monophonic policy directly, while integration code can use the
typed boundary.

Revisit this after the `midi-receiver` project produces compatible `NoteEvent`
values and the first integration firmware consumes them successfully. If the
contract holds, make `handleNoteEvent()` the public note input and move
`noteOn()` / `noteOff()` to private helpers.
