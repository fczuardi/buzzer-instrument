# Slice 002: Compare LEDC and M5.Speaker Tone Output

The first buzzer slice proved that direct LEDC/PWM on GPIO 2 can produce a
recognizable A4 tone. This slice keeps that working path as the baseline and
adds a second output path using M5Unified's `M5.Speaker.tone()` API.

The comparison is intentionally small. Button A plays the same A4 440 Hz smoke
test on the currently selected backend. Button B cycles through the direct
`ledc` backend and several `m5speaker` waveform presets. Holding Button A
returns to the LEDC baseline. The display shows the selected backend, preset,
and whether a tone is playing. Serial logs include the backend and preset names
for each tone start event.

The `m5speaker` backend uses the longer `M5.Speaker.tone()` overload with
explicit waveform samples. This mirrors the strategy used by the m5-synth prior
art more closely than the simple `M5.Speaker.tone(440, 500)` call that sounded
noisy in the first hardware test.

The two backends are separate classes. `BuzzerOutput` owns the direct LEDC
implementation, while `SpeakerToneOutput` owns the M5Unified speaker API call.
Switching backends stops any active tone and releases the inactive driver before
starting the next one.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Hardware test:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio run --target upload
env PLATFORMIO_CORE_DIR=.platformio-home pio device monitor
```

Expected result: LEDC should still play a recognizable A4. The M5.Speaker path
is the experiment under test; it should be compared by ear or tuner app after
pressing Button B to select each `m5speaker` preset, then Button A to play the
tone.

Hardware result: the LEDC backend remained the useful baseline. The
`m5speaker` backend no longer sounded like the original pure noise test, but it
still did not produce a tuner-recognized A4. It sounded closer to a distorted
note mixed with noise. That suggests the explicit waveform overload changes the
output path, but still is not clean enough on the M5StickC Plus2 buzzer for this
project's current musical-note goal.

The next hardware pass broadens the experiment from one explicit waveform to a
small preset set: `square16`, `square8`, `sine16`, `tri16`, and `pulse16`. The
goal is to learn whether the noisy output is tied to the waveform shape, the
sample count, or the M5Unified speaker path more generally.

Second hardware result: `square16`, `square8`, `sine16`, and `tri16` all sounded
very similar to each other and still were not recognized as a clean A4 by the
tuner app. `pulse16` sounded different, but also did not produce a usable note.
That makes the direct LEDC backend the clear choice for musical pitch output in
the current project.

The follow-up isolates the M5Unified speaker path even further. The firmware
temporarily stops initializing the LEDC backend at all and becomes an
`m5speaker`-only test rig. Button B cycles presets that vary volume,
`magnification`, and speaker sample rate, while Button A plays the A4 test tone.
This checks whether the noisy result is caused by clipping, output gain, sample
rate, or coexistence with the LEDC driver.

Third hardware result: the isolated `m5speaker` test changed the conclusion.
Several lower-gain presets produced clean, recognizable A4 tones. The useful
presets were `sq v64`, `sq mag32`, `sq mag16`, `sq 44k`, and `pulse16`. The
important difference from the earlier noisy tests appears to be gain staging:
the default-ish `volume=255` and `magnification=48` path likely overdrives the
buzzer output, while lower volume or lower magnification keeps the tone usable.

Decision: use M5Unified's `M5.Speaker` API as the single supported backend for
now. It integrates better with the M5/Arduino ecosystem than direct LEDC and
already provides useful concepts such as channels, duration, volume, and custom
waveforms. The implementation keeps `square16`, `volume=128`,
`magnification=32`, and `sample_rate=48000` as the first stable configuration
because that combination produced a clean A4 with a little more usable loudness
than the more conservative `volume=64`, `magnification=48` option.
