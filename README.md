# Monophonic instrument

Monophonic instrument experiments for small embedded music devices.

This repository started as the M5StickC Plus2 buzzer instrument and was renamed
after the Core Gray speaker experiment exposed the same `VoiceOutput` boundary.
It is now the home for the shared monophonic instrument policy and M5 audio
output backends.

Packages live under `packages/`. Device-level probes live under `apps/`.

## Current milestone

Run the same monophonic instrument policy through two hardware-backed M5 audio
outputs:

- convert a musical note into its frequency;
- start and stop the tone reliably;
- define predictable behavior when more than one note is requested;
- stop sounding during cleanup or disconnection;
- keep the musical policy separate from the hardware-specific output;
- preserve device calibration in the concrete buzzer and speaker backends.

The M5StickC Plus2 buzzer remains the original baseline. The M5Stack Core Gray
speaker is the second hardware validation of the same policy and `VoiceOutput`
boundary. Both also remain useful without MIDI through their local test apps;
BLE MIDI compositions live in
[embedded-music-experiments](https://github.com/fczuardi/embedded-music-experiments).

## Scope

This is not currently a general synthesizer, audio framework, or sequencer.
Reusable PlatformIO packages are being extracted based on working code and real
hardware validation.

The broader context and provisional architecture are documented in [embedded-music-experiments](https://github.com/fczuardi/embedded-music-experiments).

## Validated environments

- M5StickC Plus2 built-in buzzer;
- M5Stack Core Gray 1.0 built-in speaker;
- ESP32 and ESP32-PICO-D4;
- PlatformIO
- Arduino framework
- M5Unified

## Current Shape

`packages/monophonic-instrument` owns the shared monophonic note-event policy.
`InstrumentEventSink` accepts typed `NoteEvent` values from local or future
external producers. `MonophonicInstrument` tracks held notes with last-note
priority.

`packages/m5-tone-output` owns M5Unified speaker-like output backends. Its
`M5BuzzerToneOutput` class configures the M5StickC Plus2 buzzer and delegates
shared tone playback, waveform, and velocity-to-volume behavior to
`M5ToneOutputCore`. Its `M5CoreGrayToneOutput` class carries the M5Stack Core
Gray speaker defaults validated by the earlier speaker smoke tests while using
the same shared core.
Shared `ToneWaveformSamples` data defines the short waveform buffers consumed
by M5 audio output packages without depending on M5Unified.

`apps/plus2-buzzer-local-test` is a monophonic note-event smoke test.
Button A simulates a C4 note while pressed, and Button B cycles probe velocity
levels so the hardware can validate the current velocity-to-volume mapping.
Holding Button A for one second runs a pitch bend demo around C4.
Incoming pitch bend events adjust the active note by two semitones in either
direction by default, with a small center dead zone for touch-strip controllers
that do not return exactly to zero. `MonophonicInstrument::setPitchBendRangeSemitones()`
allows showcase firmware to tune that musical range without changing the
package default.

The current hardware result suggests that volumes `64..128` are usable on the
M5StickC Plus2 buzzer. Lower values may lose the recognizable pitch, while
larger values become distorted. `M5BuzzerToneOutput::setVelocityVolumeRange()`
allows firmware experiments to tune those limits without changing the instrument
policy.

These values are package defaults or local app choices, not fixed instrument
behavior:

| Setting | Package default | Consumer override |
| --- | --- | --- |
| Velocity-to-volume range | `64..128` | `M5BuzzerToneOutput::setVelocityVolumeRange()` |
| Pitch bend range | ±2 semitones | `MonophonicInstrument::setPitchBendRangeSemitones()` |

A showcase or device composition can calibrate both settings for its hardware
and musical purpose without forking or modifying this package.

`apps/core-gray-speaker-local-test` migrates the useful local note-sweep smoke
test from the old `m5-speaker-instrument` repo into this unified package home.
It keeps the Core Gray validation local to the device: Button A starts/stops
the selected tone, Button B advances through C3, C4, A4, C5, and C6, and Button
C toggles between `sq32` and `saw32`.

`NoteEvent`, `PitchBendEvent`, and `InstrumentEventSink` come from
`firmware-contracts` in `embedded-music-experiments`.

## Commands

```bash
pio test -d packages/monophonic-instrument -e native
pio run -d apps/plus2-buzzer-local-test
pio run -d apps/core-gray-speaker-local-test
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
pio pkg pack packages/m5-tone-output --output /home/fcz/dev/m5stick/.tmp
pio run -d apps/plus2-buzzer-local-test --target upload
pio device monitor -d apps/plus2-buzzer-local-test
pio run -d apps/core-gray-speaker-local-test --target upload
pio device monitor -d apps/core-gray-speaker-local-test
```

## CI

GitHub Actions runs the extracted package checks, native policy tests, consumer
fixtures, and both the M5StickC Plus2 and M5Stack Core Gray local app builds on
pushes and pull requests. The workflow lives at `.github/workflows/ci.yml`.
