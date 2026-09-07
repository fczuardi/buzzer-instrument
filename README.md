# Buzzer instrument

A small experiment in making a musical instrument with a passive buzzer.

The first target is the buzzer built into the M5StickC Plus2. The repository name is intentionally hardware-neutral: support for similar GPIO/PWM-driven buzzers may be added later when a real second target makes the required adaptation clear.

## First milestone

Produce one note at a time on the built-in buzzer:

- convert a musical note into its frequency;
- start and stop the tone reliably;
- define predictable behavior when more than one note is requested;
- stop sounding during cleanup or disconnection;
- keep the musical policy separate from the hardware-specific output.

The first implementation should remain useful without MIDI. Integration with [midi-receiver](https://github.com/fczuardi/midi-receiver) comes after the basic buzzer output works.

## Scope

This is not currently a general synthesizer, audio framework, sequencer, or reusable library. Those directions may be explored later, based on working code.

The broader context and provisional architecture are documented in [embedded-music-experiments](https://github.com/fczuardi/embedded-music-experiments).

## Initial environment

- M5StickC Plus2
- ESP32
- PlatformIO
- Arduino framework
- M5Unified

## Current slice

The current firmware is a monophonic note-event smoke test.
`InstrumentEventSink` accepts typed `NoteEvent` values from local or future
external producers. `MonophonicInstrument` tracks held notes with last-note
priority, and `SpeakerToneOutput` implements the hardware-neutral `VoiceOutput`
interface that renders the active note through M5Unified's `M5.Speaker` API.
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
larger values become distorted. `SpeakerToneOutput::setVelocityVolumeRange(min, max)`
allows firmware experiments to tune those limits without changing the instrument
policy.

`NoteEvent`, `PitchBendEvent`, and `InstrumentEventSink` come from the shared
`EmbeddedMusicFirmwareContracts` PlatformIO package in
`embedded-music-experiments`.

The root `library.json` packages the reusable instrument pieces as
`EmbeddedMusicBuzzerInstrument`, so a separate showcase firmware can consume the
buzzer instrument without importing this repository's local button smoke test.

## Commands

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
env PLATFORMIO_CORE_DIR=.platformio-home pio pkg pack . --output /tmp
env PLATFORMIO_CORE_DIR=.platformio-home pio run --target upload
env PLATFORMIO_CORE_DIR=.platformio-home pio device monitor
```

## CI

GitHub Actions runs native tests and the M5StickC Plus2 firmware build on pushes
and pull requests. The workflow lives at `.github/workflows/ci.yml`.
