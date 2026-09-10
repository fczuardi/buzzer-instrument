# Slice 031: Extract Note Priority Policy

## Goal

Separate the reusable monophonic held-note policy from the tone-specific
instrument state.

The AMY synth experiment exposed that the useful common boundary is not
`frequencyHz` or `VoiceOutput`. A synth backend can represent musical intent
directly as MIDI note, velocity, patch, and synth-specific controls. The common
piece is lower: when several notes are held, choose which one monophonic note
should currently sound and decide what should happen when note-on, note-off, or
panic events arrive.

## Design

Added `MonophonicNotePriority`, a small allocation-free policy class that:

- accepts MIDI note-on and note-off facts;
- treats note-on with velocity zero as note-off;
- preserves last-note priority;
- returns to the previous held note when the active note is released;
- clears held note state on `stopAll()`;
- returns `MonophonicNoteAction` values containing only action type, MIDI note,
  and velocity.

`MonophonicInstrument` now delegates held-note state to
`MonophonicNotePriority` and keeps the tone-specific concerns:

- MIDI note to frequency conversion;
- pitch-bend range and dead zone;
- waveform selection;
- `VoiceAction` values for `VoiceOutput` implementations.

This keeps existing tone and buzzer consumers working while making the
note-priority policy usable by synth backends that should not depend on
frequency or waveform output.

## Verification

```sh
pio test -e native
```

The first run installed Unity after the first test had already failed with a
missing `unity.h`. Re-running after PlatformIO installed the dependency passed:

- 49 test cases succeeded;
- existing `MonophonicInstrument` and `MonophonicInstrumentSink` tests still
  passed;
- new direct `MonophonicNotePriority` tests passed.

## Limits

This slice does not change the public `MonophonicInstrumentSink` shape and does
not yet make the AMY package consume the new policy. That is the next
cross-repository validation step.
