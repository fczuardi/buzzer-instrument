# Rename Firmware Contracts Consumer

## Goal

Consume the renamed shared firmware contract package after
`embedded-music-experiments` changed the PlatformIO package name from
`EmbeddedMusicFirmwareContracts` to `firmware-contracts`.

## Design

Only the active dependency declarations changed. The C++ contract types remain
the same: `NoteEvent`, `PitchBendEvent`, and `InstrumentEventSink`.

The package dependency now pins `firmware-contracts` to umbrella commit
`c9bde598cff9`, where the package rename was introduced.

Historical devlogs keep the names that were true when those slices ran.

## Verification

Commands run:

```bash
pio test -d packages/monophonic-instrument -e native
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
pio run -d apps/plus2-buzzer-local-test
pio run -d apps/core-gray-speaker-local-test
```

The host suite passed 44 test cases. Both hardware app builds passed after
clearing stale PlatformIO `libdeps` caches, and the logs showed
`firmware-contracts@0.1.0+sha.c9bde59` being installed.

No hardware upload is required for this naming-only slice.
