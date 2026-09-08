# Plus2 buzzer local test

Local M5StickC Plus2 button-driven firmware for probing the monophonic
instrument and buzzer output packages without BLE MIDI.

This app consumes the package sources from this repository:

```ini
lib_deps =
  monophonic-instrument=file://../../packages/monophonic-instrument
  m5-tone-output=file://../../packages/m5-tone-output
```

Commands:

```bash
pio run -d apps/plus2-buzzer-local-test
pio run -d apps/plus2-buzzer-local-test --target upload
pio device monitor -d apps/plus2-buzzer-local-test
```
