# Slice 003: MIDI Note Frequencies

The previous slices proved that the buzzer can play a clean A4 through
M5Unified's `M5.Speaker` API. This slice moves one layer up: instead of hard
coding `440 Hz`, the firmware starts from MIDI note numbers and calculates the
frequency to play.

The new `MidiNote` module is deliberately small and pure C++. It has no Arduino
or M5 dependency. MIDI note 69 is A4 at 440 Hz, and every semitone changes the
frequency by the twelfth root of two. Keeping this conversion separate makes it
easy to reuse when the receiver later sends typed note events.

The firmware initially cycled through a C major scale, then moved to a more
useful piano-like navigation model. It now starts on C4, Button A advances one
semitone, Button B cycles waveform, and holding Button A resets to C4. During
spectrum inspection the note duration was temporarily raised to four seconds;
after the screenshots were collected, it settled at 300 ms for a more responsive
interaction. The display shows the waveform, note name, MIDI note number,
calculated frequency, output backend, and playback state. Serial logs include
the same note and frequency fields.

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

Expected result: the firmware starts at C4. Button A walks upward by semitone
until C8, Button B cycles `square16`, `pulse16`, `saw16`, and `sawdown16`, and
holding Button A returns to C4. Each selected note plays for 300 ms, and a
tuner app should recognize pitches closely enough in the buzzer's useful range.

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
more useful range. The upper limit stays at C8 because C9 is too piercing on
this buzzer.

The waveform was then changed from `square16` to `pulse16` for a timbre test.
Earlier experiments suggested pulse can stay recognizable while sounding more
distinct than the square baseline.

The UI now cycles waveform with Button B so the chromatic C4-C8 range can be
compared across `square16`, `pulse16`, `saw16`, and `sawdown16` without
rebuilding firmware.

Hardware result: `saw16` was the best sounding waveform in the C5-C8 range.
The firmware now starts on `saw16` while still allowing Button B to cycle
through the other waveforms for comparison.
