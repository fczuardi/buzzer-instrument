# Slice 011: Use Shared Firmware Contracts

The umbrella repository now exposes `NoteEvent` and `InstrumentEventSink` as a
PlatformIO library package. This slice makes `buzzer-instrument` consume those
contracts instead of keeping local copies.

`platformio.ini` declares `EmbeddedMusicFirmwareContracts` in `lib_deps`, pinned
to the exact umbrella commit that packaged the contracts. The M5 firmware
environment extends that shared dependency list with `M5Unified`; the native
test environment uses only the shared contracts and pure local code.

The local `include/NoteEvent.h` and `include/InstrumentEventSink.h` files were
removed. Existing includes remain unchanged:

```cpp
#include "NoteEvent.h"
#include "InstrumentEventSink.h"
```

This is the useful part of the PlatformIO package boundary: project code does
not need to know where the headers live inside the provider repository.

This slice deliberately does not add BLE integration. The local Button A smoke
test still produces Note On and Note Off events. The change being tested is only
that the event vocabulary now comes from the shared package.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: native tests pass, firmware builds, and PlatformIO resolves
`EmbeddedMusicFirmwareContracts` from the pinned Git commit.
