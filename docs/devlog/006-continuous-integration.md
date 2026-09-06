# Slice 006: Continuous Integration

The firmware now has enough pure logic and hardware-facing code that a small CI
check is useful. This slice adds a GitHub Actions workflow that repeats the same
commands we run locally before closing a slice.

The workflow has two jobs. `native-tests` runs the Unity tests for pure C++
modules such as MIDI note conversion and monophonic note-event state.
`firmware-build` compiles the M5StickC Plus2 firmware to catch Arduino,
M5Unified, and PlatformIO integration errors.

Both jobs install PlatformIO `6.1.19`, use Python `3.13`, and set
`PLATFORMIO_CORE_DIR=.platformio-home`. Keeping the same PlatformIO version and
core directory as local development makes CI failures easier to compare with
local runs.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: pushes and pull requests run native tests and firmware build
without needing a connected M5Stick.
