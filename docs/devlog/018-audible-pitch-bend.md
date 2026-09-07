# Slice 018: Audible Pitch Bend

The instrument package already accepted `PitchBendEvent`, but the event did not
change sound. This slice makes pitch bend audible in the monophonic policy.

`MonophonicInstrument` now stores the latest centered bend value and applies it
when calculating the active note frequency. The initial musical choice is fixed
and conservative:

```text
range: +/-2 semitones
dead zone: +/-128
```

The dead zone absorbs small touch-strip return values such as `64`, which were
observed on the Arturia controller. Values outside the dead zone are mapped
linearly across the MIDI pitch bend range `-8192..8191`.

`VoiceAction` now carries the calculated `frequencyHz`, and `VoiceOutput`
receives that frequency directly. This keeps pitch bend policy in the
instrument instead of making the M5 speaker backend understand MIDI pitch bend.

Native tests cover center, minimum, maximum, dead-zone behavior, bend received
before a note starts, bend during an active note, and reset through `stopAll()`.

The local button firmware also gained a manual bend demo: holding Button A for
one second starts a C4 note and steps through center, full down, center, full up,
and center again. The hardware test passed on the M5StickC Plus2: the pitch
variation was audible in both directions.
