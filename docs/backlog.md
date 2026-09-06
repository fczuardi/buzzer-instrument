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

## Extract Velocity-To-Volume Mapping When It Grows

`SpeakerToneOutput` currently maps MIDI velocity to speaker volume with a simple
linear range. The behavior is deterministic, but it lives inside the M5 speaker
backend and is only validated through hardware tests.

If we add non-linear velocity curves, reusable presets, or a second audio
backend, move this mapping to pure C++ code with native tests. Cover at least
velocity 0/1 mapping to the minimum, velocity 127 mapping to the maximum, a
midpoint, an inverted configured range, and a constant range such as `{100, 100}`.
