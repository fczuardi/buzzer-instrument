# Slice 020: Plan Monophonic Instrument Repository

The buzzer package has reached the point where it is no longer just a buzzer
experiment. It contains a reusable monophonic instrument policy, MIDI note
helpers, typed event sinks, waveform vocabulary, pitch bend behavior, velocity
mapping, and one concrete M5 buzzer output.

The Core Gray speaker experiment independently reached a compatible
`VoiceOutput` boundary. That second consumer makes the shared contract concrete
enough to extract without guessing: both devices need the same monophonic
instrument policy, while their audio outputs should remain separate hardware
backends.

The chosen migration path is to rename this repository from
`buzzer-instrument` to `monophonic-instrument` on GitHub, then reorganize the
contents into a small multi-package repository:

```text
monophonic-instrument/
  packages/
    embedded-music-monophonic/
    embedded-music-m5-buzzer-output/
    embedded-music-m5-speaker-output/

  apps/
    m5stickc-plus2-buzzer-local-test/
    m5stack-core-gray-speaker-local-test/

  docs/devlog/
```

The package names intentionally include `m5` for both output backends. This is
more consistent than mixing a generic `buzzer-output` name with an
M5-specific `m5speaker-output` name, and it reflects the current evidence: both
backends are calibrated and validated inside the M5Stack/M5Unified ecosystem.

The planned boundaries are:

| Package | Responsibility |
| --- | --- |
| `embedded-music-monophonic` | `MidiNote`, `ToneWaveform`, `VoiceOutput`, `MonophonicInstrument`, and `MonophonicInstrumentSink` |
| `embedded-music-m5-buzzer-output` | M5 buzzer/PWM output backend and buzzer-specific calibration |
| `embedded-music-m5-speaker-output` | M5Unified `M5.Speaker` output backend and speaker-specific calibration |

Concrete device choices should live in app-level configuration, not package
names. For example, the Plus2 buzzer and Core Gray speaker can set pins,
channels, volume ranges, tested frequency regions, and local button/display UI
from their app directories while sharing the same output package when the
underlying backend is the same.

The umbrella repository should keep composing showcase firmware from reusable
pieces, for example:

```ini
lib_deps =
  file://../../monophonic-instrument/packages/embedded-music-monophonic
  file://../../monophonic-instrument/packages/embedded-music-m5-buzzer-output
  file://../../midi-receiver
```

or:

```ini
lib_deps =
  file://../../monophonic-instrument/packages/embedded-music-monophonic
  file://../../monophonic-instrument/packages/embedded-music-m5-speaker-output
  file://../../midi-receiver
```

This keeps the umbrella responsible for LEGO-style composition, while the
instrument repository owns the musical policy and output backends.

Operational plan:

1. Rename the GitHub repository `buzzer-instrument` to
   `monophonic-instrument`.
2. Adjust the local `jj`/Git remote to the renamed repository.
3. Move the current reusable buzzer code into `packages/`.
4. Move the current local buzzer firmware into `apps/`.
5. Import the Core Gray speaker output and local sweep app from
   `m5-speaker-instrument`.
6. Update PlatformIO manifests and CI so each package/app builds in isolation.
7. Update the umbrella repository to consume the new package paths.
8. Mark the old `m5-speaker-instrument` repository as superseded after the
   migration is verified.

This slice does not perform the migration yet. It records the repository-level
decision before the GitHub rename so the next refactor has an explicit target
shape.
