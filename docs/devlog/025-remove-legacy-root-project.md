# Slice 025: Remove Legacy Root Project

The repository no longer needs to preserve the old combined
`EmbeddedMusicBuzzerInstrument` package or root PlatformIO project. The only
current consumers are our own experiments, and the M5StickC Plus2 BLE showcase
already builds from the extracted `monophonic-instrument` and
`m5-tone-output` packages.

This slice makes `packages/` the source of truth and moves the local Plus2
button firmware to:

```text
apps/plus2-buzzer-local-test/
```

The app consumes the two package paths directly instead of compiling duplicated
root `include/`, `src`, and `test` trees. The M5StickC Plus2 board definition
also moves under the app. The legacy root `library.json` and root
`platformio.ini` are removed, and CI now validates only the package builds plus
the local app build.

Validation targets:

```bash
pio test -d packages/monophonic-instrument -e native
pio run -d apps/plus2-buzzer-local-test
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
pio pkg pack packages/m5-tone-output --output /home/fcz/dev/m5stick/.tmp
```

The old root native tests were duplicates of the package tests and are removed
with the root project.

Verified result:

- `packages/monophonic-instrument` native tests passed with 44/44 test cases.
- `apps/plus2-buzzer-local-test` built successfully from its app directory.
- `monophonic-instrument` and `m5-tone-output` package archives packed
  successfully into `/home/fcz/dev/m5stick/.tmp`.
- A fresh temporary Arduino consumer built successfully from the two local
  package paths, proving there is no root `include/` or `src/` fallback.
- `embedded-music-experiments` showcase 1 still built successfully against the
  refreshed package archives.
