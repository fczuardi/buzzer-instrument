# Monophonic instrument

Monophonic instrument experiments for small embedded music devices.

This repository started as the M5StickC Plus2 buzzer instrument and was renamed
after the Core Gray speaker experiment exposed the same `VoiceOutput` boundary.
It is now the home for the shared monophonic instrument policy and M5 audio
output backends.

The current root firmware still targets the buzzer built into the M5StickC
Plus2. The package/app layout is being migrated in small slices under
`packages/` and `apps/`.

## First milestone

Produce one note at a time on the built-in buzzer:

- convert a musical note into its frequency;
- start and stop the tone reliably;
- define predictable behavior when more than one note is requested;
- stop sounding during cleanup or disconnection;
- keep the musical policy separate from the hardware-specific output.

The first implementation should remain useful without MIDI. Integration with [midi-receiver](https://github.com/fczuardi/midi-receiver) comes after the basic buzzer output works.

## Scope

This is not currently a general synthesizer, audio framework, or sequencer.
Reusable PlatformIO packages are being extracted based on working code and real
hardware validation.

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
Shared `ToneWaveformSamples` data defines the short waveform buffers consumed
by M5 audio output packages without depending on M5Unified.
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
larger values become distorted. `SpeakerToneOutput::setVelocityVolumeRange()`
allows firmware experiments to tune those limits without changing the instrument
policy.

These values are package defaults, not fixed instrument behavior:

| Setting | Package default | Consumer override |
| --- | --- | --- |
| Velocity-to-volume range | `64..128` | `SpeakerToneOutput::setVelocityVolumeRange()` |
| Pitch bend range | ±2 semitones | `MonophonicInstrument::setPitchBendRangeSemitones()` |

A showcase or device composition can calibrate both settings for its hardware
and musical purpose without forking or modifying this package.

`NoteEvent`, `PitchBendEvent`, and `InstrumentEventSink` come from the shared
`EmbeddedMusicFirmwareContracts` PlatformIO package in
`embedded-music-experiments`.

The root `library.json` still packages the legacy buzzer instrument as
`EmbeddedMusicBuzzerInstrument`, so a separate showcase firmware can consume
the current buzzer instrument without importing this repository's local button
smoke test. The `packages/monophonic-instrument` package owns the shared policy,
and `packages/m5-buzzer-output` owns the M5StickC Plus2 buzzer backend. The
next umbrella slice can move the BLE MIDI buzzer showcase from the legacy
combined package to those two packages.

## Commands

```bash
pio test -e native
pio test -d packages/monophonic-instrument -e native
pio run
pio pkg pack . --output /home/fcz/dev/m5stick/.tmp
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
pio pkg pack packages/m5-buzzer-output --output /home/fcz/dev/m5stick/.tmp
pio run --target upload
pio device monitor
```

## CI

GitHub Actions runs native tests, the M5StickC Plus2 firmware build, the legacy
root package check, and the extracted `monophonic-instrument` and
`m5-buzzer-output` package checks on pushes and pull requests. The workflow
lives at `.github/workflows/ci.yml`.
