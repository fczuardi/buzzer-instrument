# Slice 012: Package Buzzer Instrument

The buzzer experiment now has clear reusable pieces: note/frequency helpers,
monophonic instrument policy, a shared event sink implementation, and an
M5Unified speaker output. This slice packages those pieces as a PlatformIO
library named `EmbeddedMusicBuzzerInstrument`.

The root `library.json` declares the package because Git dependencies install
the repository root. The manifest exports only the reusable headers, sources,
README, this devlog, and the manifest. It uses `build.srcFilter` so consumers
compile the instrument modules but not the local smoke-test firmware in
`src/main.cpp`.

The package depends on `EmbeddedMusicFirmwareContracts` for `NoteEvent` and
`InstrumentEventSink`, and on `m5stack/M5Unified@0.2.17` for the concrete
`SpeakerToneOutput`. For now the whole package is declared as Arduino/ESP32
because it includes that M5 speaker backend. The pure policy classes can be
split into a more portable package later if a non-M5 consumer appears.

The local firmware remains useful as a standalone hardware smoke test. Button A
still simulates Note On/Off C4, and Button B still toggles `saw32`/`sq32`.

CI now validates the package in two ways: `pio pkg pack` checks the package
manifest/export rules, and a temporary PlatformIO consumer builds against
`file://${{ github.workspace }}` to prove another firmware can include and link
the instrument package.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
env PLATFORMIO_CORE_DIR=.platformio-home pio pkg pack . --output /tmp
```

Expected result: native tests pass, the smoke-test firmware builds, and the
package tarball excludes `src/main.cpp`.
