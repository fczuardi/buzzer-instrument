# Slice 003: MIDI Note Frequencies

The previous slices proved that the buzzer can play a clean A4 through
M5Unified's `M5.Speaker` API. This slice moves one layer up: instead of hard
coding `440 Hz`, the firmware starts from MIDI note numbers and calculates the
frequency to play.

The new `MidiNote` module is deliberately small and pure C++. It has no Arduino
or M5 dependency. MIDI note 69 is A4 at 440 Hz, and every semitone changes the
frequency by the twelfth root of two. Keeping this conversion separate makes it
easy to reuse when the receiver later sends typed note events.

The firmware initially cycled through a C major scale, then moved through
several piano-like navigation experiments. It currently starts on C3, Button A
advances one degree in the C major scale up to C7, Button B cycles waveform
tables, and holding Button A resets to C3. During spectrum inspection the note
duration was temporarily raised to four seconds; after the screenshots were
collected, it settled at 300 ms for a more responsive interaction. The display
shows the waveform, note name, MIDI note number, calculated frequency, output
backend, and playback state. Serial logs include the same note and frequency
fields.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
```

Hardware test:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run --target upload
env PLATFORMIO_CORE_DIR=.platformio-home pio device monitor
```

Expected result: the firmware starts at C3 for the current lower-bound test.
Button A walks upward through C major until C7, Button B toggles between `sq32`
and `saw32`, and holding Button A returns to C3. Each selected note plays for
300 ms, and a tuner app should recognize pitches closely enough in the buzzer's
useful range.

Follow-up: the output waveform briefly changed from `square16` to `sine16` to
see whether a sine-like table would reduce harmonic energy. On hardware, that
returned to a noisy sound, so the firmware went back to the tested `square16`
waveform. For this passive buzzer, the simpler on/off waveform is currently the
more useful musical output.

Hardware spectrum notes showed that C5 did not produce a visible fundamental
around 523 Hz, while stronger energy appeared in upper harmonics around several
kHz. The default demo scale therefore moved up to C6-C7, closer to the range
where the small buzzer appears to radiate more efficiently.

The lower limit was later raised from C0 to C4 because C0-C2 were only useful as
diagnostics and sounded like noise rather than musical notes. The lower limit
then settled at C4 so the UI can still explore the transition into the buzzer's
more useful range. The upper limit moved from C8 to C7 because C8 sounded both
too high and perceptually louder, likely due to buzzer resonance.

The waveform was then changed from `square16` to `pulse16` for a timbre test.
Earlier experiments suggested pulse can stay recognizable while sounding more
distinct than the square baseline.

The UI now cycles waveform with Button B so the C4-C7 range can be compared
across waveforms without rebuilding firmware. The first comparison included
square and saw tables at 8, 16, and 32 samples per cycle.

The note navigation was then simplified from chromatic semitones to the C major
scale from C4 to C7. This makes the toy easier to play while still exercising
the MIDI-note-to-frequency conversion.

For one more lower-bound check, the scale start moved temporarily down to C3.
This keeps the same C-major navigation while making it easy to confirm whether
anything below C4 is musically useful on the physical buzzer.

Hardware result: `saw16` was the best sounding waveform in the C5-C8 range, but
32-sample tables improved C3. After comparing 32-, 64-, and 128-sample tables,
32 was the sweet spot. The current firmware keeps only `sq32` and `saw32` in the
live selector.
