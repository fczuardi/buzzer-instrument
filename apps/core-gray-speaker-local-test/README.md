# Core Gray speaker local test

Local M5Stack Core Gray note-sweep firmware for validating the shared
`m5-tone-output` package on the built-in speaker.

Controls:

- Button A starts or stops the selected tone.
- Button B advances through C3, C4, A4, C5, and C6.
- Button C toggles between `sq32` and `saw32`.

Commands:

```bash
pio run -d apps/core-gray-speaker-local-test
pio run -d apps/core-gray-speaker-local-test --target upload
pio device monitor -d apps/core-gray-speaker-local-test
```
