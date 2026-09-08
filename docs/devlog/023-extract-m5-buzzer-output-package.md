# Slice 023: Extract M5 Buzzer Output Package

The shared monophonic policy now lives in `packages/monophonic-instrument`, but
the M5StickC Plus2 buzzer backend still only existed inside the legacy root
package. That left the first BLE MIDI showcase blocked on the old combined
`EmbeddedMusicBuzzerInstrument` package.

This slice creates `packages/m5-buzzer-output` as a focused PlatformIO package
for the existing `SpeakerToneOutput` backend. The package includes the concrete
M5Unified speaker API dependency. Consumers must also depend on
`monophonic-instrument` for the `VoiceOutput` and `ToneWaveform` contracts while
the shared package is consumed from local paths or archives.

The legacy root package and local button firmware remain in place. That keeps
the already validated root app available while the umbrella showcase is moved
to the new two-package composition in a later slice.

Validation targets:

```bash
pio test -e native
pio test -d packages/monophonic-instrument -e native
pio run
pio pkg pack packages/m5-buzzer-output --output /home/fcz/dev/m5stick/.tmp
```

A temporary Arduino consumer should also build with both package paths:

```ini
lib_deps =
  monophonic-instrument=file:///home/fcz/dev/m5stick/monophonic-instrument/packages/monophonic-instrument
  m5-buzzer-output=file:///home/fcz/dev/m5stick/monophonic-instrument/packages/m5-buzzer-output
```

Expected result: the consumer can include `MonophonicInstrument`,
`MonophonicInstrumentSink`, and `SpeakerToneOutput`, then compose the same
`BleMidiInput -> MonophonicInstrumentSink -> SpeakerToneOutput` shape used by
the current buzzer showcase.

Verified result:

- Root native tests passed with 42/42 test cases.
- `packages/monophonic-instrument` native tests passed with 42/42 test cases.
- The M5StickC Plus2 root firmware build passed.
- `packages/m5-buzzer-output` packed as
  `/home/fcz/dev/m5stick/.tmp/m5-buzzer-output-0.1.0.tar.gz`.
- A temporary Arduino consumer built successfully from local package paths.
- A temporary Arduino consumer built successfully from packed archives for both
  `monophonic-instrument` and `m5-buzzer-output`.

The first consumer attempt also showed that a relative `file://` dependency
inside `m5-buzzer-output/library.json` is not portable after PlatformIO installs
the package into a consumer project. For now, local and archive consumers list
both packages explicitly. A future release or registry URL can replace that
manual pairing.
