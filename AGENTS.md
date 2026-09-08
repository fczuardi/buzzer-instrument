# Repository Guidelines

This repository follows the shared guidelines in the parent `m5stick`
directory.

## Project Scope

Build shared monophonic instrument policy and M5 audio output packages from
small hardware-backed experiments. Keep the package APIs simple and revise them
freely while the project is still young.

Keep these responsibilities separate:

- note/frequency and instrument policy;
- monophonic voice state;
- hardware-specific buzzer output;
- display and serial diagnostics.

Do not import receiver code or create shared libraries until this project has a
working need for the same contract.

## Project Structure

- `packages/monophonic-instrument/`: shared pure instrument policy and native
  tests.
- `packages/m5-tone-output/`: M5Unified tone output backends, including the
  M5StickC Plus2 buzzer output and M5Stack Core Gray speaker output.
- `apps/`: local hardware probes that consume packages.
- `docs/devlog/`: one Markdown chapter per implementation slice.

## Build, Test, and Development Commands

Use PlatformIO for package and app validation:

- `pio test -d packages/monophonic-instrument -e native`: run package native
  tests.
- `pio run -d apps/plus2-buzzer-local-test`: build the local M5StickC Plus2
  firmware probe.
- `pio run -d apps/plus2-buzzer-local-test --target upload`: upload to the
  connected device.
- `pio device monitor -d apps/plus2-buzzer-local-test`: read serial output.
- `pio run -d apps/core-gray-speaker-local-test`: build the local M5Stack Core
  Gray speaker probe.

## Coding Style & Naming Conventions

Use explicit C++ with small modules and descriptive constants, for example
`MIDDLE_C_MIDI_NOTE`, `A4_FREQUENCY_HZ`, and `MAX_ACTIVE_VOICES`. Keep hardware
calls isolated from pure musical logic so native tests can cover the latter.

## Testing Guidelines

Prefer native tests for deterministic logic before testing on hardware. Good
early tests include MIDI-note-to-frequency conversion, monophonic note priority,
and `stopAll()` behavior. Hardware slices should also record observed behavior
in the devlog.

## Commit & Pull Request Guidelines

Use `jj` for normal version control. Keep commits aligned with tested slices and
use concise imperative messages, for example `firmware: add buzzer tone smoke
test`. Before pushing, run the relevant tests/build and update the devlog.
