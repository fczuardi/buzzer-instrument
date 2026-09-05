# Repository Guidelines

This repository follows the shared guidelines in the parent `m5stick`
directory.

## Project Scope

Build a small buzzer-based instrument experiment for the M5StickC Plus2. Start
without MIDI. First prove that the internal buzzer can produce controlled notes,
then add musical policy in small steps.

Keep these responsibilities separate:

- note/frequency and instrument policy;
- monophonic voice state;
- hardware-specific buzzer output;
- display and serial diagnostics.

Do not import receiver code or create shared libraries until this project has a
working need for the same contract.

## Project Structure

Expected structure after initialization:

- `src/`: firmware entry point and runtime modules.
- `include/`: project headers for reusable modules.
- `test/`: native tests for pure logic such as note-to-frequency conversion.
- `docs/devlog/`: one Markdown chapter per implementation slice.
- `platformio.ini`: PlatformIO environments and dependencies.

## Build, Test, and Development Commands

Use PlatformIO once the project is initialized:

- `env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native`: run native
  tests.
- `env PLATFORMIO_CORE_DIR=.platformio-home pio run`: build the M5StickC Plus2
  firmware.
- `env PLATFORMIO_CORE_DIR=.platformio-home pio run --target upload`: upload to
  the connected device.
- `env PLATFORMIO_CORE_DIR=.platformio-home pio device monitor`: read serial
  output.

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
