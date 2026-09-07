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

The main structural risk is remote consumption of subpackages from a monorepo.
The local `file://` examples are enough for sibling checkouts, but they do not
prove that a published umbrella showcase can install one package from a Git
repository subdirectory. A plain Git dependency normally expects a PlatformIO
package at the repository root, not multiple independent `library.json` files
inside `packages/`.

Before moving every backend, the migration should therefore prove the package
delivery mechanism with only `packages/embedded-music-monophonic`. The first
micro-slice after the GitHub rename should:

1. Create `packages/embedded-music-monophonic`.
2. Validate local consumption with `file://`.
3. Define and validate one remote consumption strategy.
4. Move the hardware output packages only after that strategy is proven.

Possible remote distribution strategies:

| Strategy | Tradeoff |
| --- | --- |
| PlatformIO Registry packages | Clean consumer experience, but adds registry workflow early |
| GitHub Release archives per package | Keeps the monorepo while giving PlatformIO versioned package URLs |
| Single temporary root package | Simplest short-term path, but weakens independent package boundaries |
| Local-only consumption for now | Good for migration speed, but does not solve published showcase reproducibility |

The preferred direction is GitHub Release archives per package, for example
`embedded-music-monophonic-0.1.0.tar.gz`,
`embedded-music-m5-buzzer-output-0.1.0.tar.gz`, and
`embedded-music-m5-speaker-output-0.1.0.tar.gz`. This preserves the monorepo
while giving consumers fixed, reproducible dependency URLs without requiring an
immediate registry publication process.

Two names remain intentionally provisional at the shared boundary:
`ToneWaveform::Square32` and `ToneWaveform::Saw32`. They describe the current
32-sample wavetable representation used by both backends. Before freezing a
public shared package, the extraction should decide whether the semantic
contract should instead be `Square` and `Saw`, leaving each output backend to
choose its rendering method.

The Core Gray speaker migration should preserve its empirical history rather
than copy only the final files. The speaker repository records hardware
findings, spectrometer observations, and a fixed state-transition bug. The
migration should use a history-preserving method such as a subtree import or a
merge of unrelated histories, then leave `m5-speaker-instrument` archived with
a final README note pointing to `monophonic-instrument`.

Migration is complete only when:

- native tests for the monophonic policy pass;
- each package can be packed in isolation;
- both local apps compile;
- the Plus2 buzzer still emits sound on hardware;
- the Core Gray speaker still emits sound on hardware;
- the Plus2 BLE showcase compiles with fixed remote dependencies;
- both outputs obey the same `VoiceOutput` contract, including failed
  `startNote()` requests while already playing;
- no backend includes or knows `MonophonicInstrument`;
- no app redefines the monophonic policy.

Operational plan:

1. Rename the GitHub repository `buzzer-instrument` to
   `monophonic-instrument`.
2. Adjust the local `jj`/Git remote to the renamed repository.
3. Create only `packages/embedded-music-monophonic` and prove local package
   consumption with `file://`.
4. Prove the chosen remote package consumption mechanism.
5. Move the current reusable buzzer code into `packages/`.
6. Move the current local buzzer firmware into `apps/`.
7. Import the Core Gray speaker output and local sweep app from
   `m5-speaker-instrument`.
8. Update PlatformIO manifests and CI so each package/app builds in isolation.
9. Update the umbrella repository to consume the new package paths.
10. Mark the old `m5-speaker-instrument` repository as superseded after the
   migration is verified.

This slice does not perform the migration yet. It records the repository-level
decision before the GitHub rename so the next refactor has an explicit target
shape.
