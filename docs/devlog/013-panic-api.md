# Slice 013: Panic API

The BLE MIDI buzzer showcase exposed a route failure that the receiver cannot
observe: an Android bridge can keep BLE connected while losing its upstream USB
MIDI controller. If the bridge sends no Note Off, no cleanup control message,
and no BLE disconnect, the instrument needs a local escape hatch.

This slice adds `MonophonicInstrumentSink::panic()`. It clears all held note
state in `MonophonicInstrument` and unconditionally stops the `VoiceOutput`.

`panic()` is deliberately separate from `onDisconnected()`. A disconnect remains
a transport lifecycle event, while panic means "silence everything now" without
claiming anything about BLE, USB, or MIDI routing.

`onDisconnected()` now delegates to `panic()`, preserving the previous defensive
behavior. A new native test verifies that panic clears held notes and stops the
output even without a disconnect event.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: native tests pass and the firmware behavior remains unchanged.
