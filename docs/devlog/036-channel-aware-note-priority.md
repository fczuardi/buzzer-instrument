# Channel-Aware Note Priority

## Goal

Preserve note ownership when one global monophonic policy receives notes from
multiple MIDI channels.

## Design

Held-note identity is now `(midiChannel, midiNote)`, rather than only the note
number. The priority policy remains globally monophonic: a newer note replaces
the sounding note, and releasing it can return to the most recent held note,
including one from another channel.

The active channel is exposed so consumers can restrict channel-scoped global
controls such as pitch bend without breaking Note Off handling. Existing direct
note APIs retain channel zero as their default; typed `NoteEvent` paths preserve
the event channel.

## Verification

Native tests cover cross-channel fallback and ensure a Note Off from the wrong
channel cannot release a held note. All 50 native test cases passed.

The AMY dual-channel consumer was rebuilt against `monophonic-instrument@0.1.3`
and validated on the Core Gray. A sequence involving channel switching,
fallback, and key release no longer left a note sounding without a key held.
