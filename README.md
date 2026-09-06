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
`MonophonicInstrument` accepts MIDI-like `noteOn()` and `noteOff()` calls and
tracks one active note at a time. Button A simulates a C4 note while pressed,
Button B toggles `sq32`/`saw32`, and the active note is rendered through
M5Unified's `M5.Speaker` API.

## Commands

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
env PLATFORMIO_CORE_DIR=.platformio-home pio run --target upload
env PLATFORMIO_CORE_DIR=.platformio-home pio device monitor
```

## CI

GitHub Actions runs native tests and the M5StickC Plus2 firmware build on pushes
and pull requests. The workflow lives at `.github/workflows/ci.yml`.
