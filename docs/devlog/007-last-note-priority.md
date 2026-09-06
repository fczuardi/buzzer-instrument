# Slice 007: Last-Note Priority

The first MIDI-like event slice remembered only one active note. That was enough
for Button A, but it would fail a normal keyboard sequence: Note On C4, Note On
E4, Note Off E4 should return to C4, not silence the instrument.

This slice changes `MonophonicInstrument` to keep a small fixed-size list of
held notes. The active note is always the most recent held note. Releasing the
active note falls back to the previous held note; releasing a non-current note
only updates the held-note list and does not ask the audio backend to stop.

`noteOn()` now deduplicates before pushing a note to the top of priority. That
prevents repeated Note On messages from leaving duplicate held entries that
would later become stuck notes. MIDI note `0` remains valid because held-note
validity is tracked by `heldNoteCount_`, not by a sentinel note value.

The note event methods now return `VoiceAction`: `StartNote`, `StopNote`, or
`None`. This lets the instrument state tell the hardware layer exactly when the
voice should start, stop, or stay unchanged.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: Button A still behaves as before for the hardware smoke test,
while native tests prove the richer multi-note priority behavior.

Follow-up: `VoiceAction` now carries the exact MIDI note that should be started
or stopped, and the hardware layer executes that note directly instead of
re-reading instrument state. This keeps the command safe if voice actions later
move through a queue.

The held-note list has a fixed capacity of 16 notes. When capacity is exceeded,
the oldest held note is discarded and the most recent notes keep their priority.
That policy keeps the event path allocation-free and is now covered by a native
test.
