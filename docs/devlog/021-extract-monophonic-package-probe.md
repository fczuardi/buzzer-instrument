# Slice 021: Extract Monophonic Package Probe

After the GitHub repository rename, the first migration step is deliberately
small: create only the `embedded-music-monophonic` subpackage and prove that it
can behave as an independent PlatformIO library.

The package currently contains the shared musical layer:

- `MidiNote`
- `ToneWaveform`
- `VoiceOutput`
- `MonophonicInstrument`
- `MonophonicInstrumentSink`

The package does not contain `SpeakerToneOutput` or any M5 hardware backend.
Those stay in the root buzzer app/package for this slice so the existing
firmware and legacy package checks continue to protect current behavior while
the subpackage mechanics are tested.

This creates temporary duplication between the root package and
`packages/embedded-music-monophonic`. That duplication is intentional and has a
short deadline: the next migration slices should move the root app and output
backend onto the package path after local and remote consumption are proven.

Validation targets:

```bash
pio test -d packages/embedded-music-monophonic -e native
pio pkg pack packages/embedded-music-monophonic --output /tmp
```

A temporary consumer should also build against the package through `file://`.
This proves the local LEGO-style consumption path used by sibling checkouts.

The remaining open risk is remote consumption. A Git URL to the monorepo does
not automatically select one package subdirectory, so the next packaging slice
should prove the preferred release-archive strategy before the umbrella
showcases depend on these package paths remotely.
