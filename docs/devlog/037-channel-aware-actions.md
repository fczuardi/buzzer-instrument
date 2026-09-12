# Channel-Aware Actions

## Goal

Carry the complete note identity through the monophonic policy so consumers
can make decisions before starting a fallback note.

## Design

`MonophonicNoteAction` now carries `midiChannel` together with note and
velocity. `VoiceAction` preserves that same field while remaining independent
of frequency-oriented output. Existing direct note APIs continue to use
channel zero.

This matters when one global monophonic instrument serves multiple MIDI
channels. A fallback after a Note Off can now identify the channel whose note
is returning before a backend starts it. The AMY facade uses that identity to
select the matching patch before sending the fallback Note On.

## Verification

Native tests cover channel identity on normal actions, cross-channel fallback,
and cleanup actions. All 51 native test cases passed. The AMY dual-channel firmware builds against the local
package checkout with the same 129,811-byte IRAM usage and 1,261 bytes free.

The public package version advances to `0.1.4` because the action structures
now expose the channel identity.

Physical validation of the fallback patch selection remains pending; the
existing hardware validation covered note ownership and cleanup behavior.
