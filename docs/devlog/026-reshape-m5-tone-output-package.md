# Slice 026: Reshape M5 Tone Output Package

The early extracted output package was named `m5-buzzer-output` and exposed
`SpeakerToneOutput`. That matched the first hardware target, but the next target
is the same monophonic instrument behavior on an M5Stack Core Gray speaker. The
useful abstraction is not "buzzer only"; it is M5Unified tone output with
device-specific speaker configuration.

This slice replaces `packages/m5-buzzer-output` with `packages/m5-tone-output`.
The new package exposes:

- `M5ToneOutputCore`, which owns shared M5Unified tone playback,
  velocity-to-volume mapping, waveform selection, volume state, and
  `VoiceOutput` behavior.
- `M5BuzzerToneOutput`, which configures the M5StickC Plus2 buzzer pin and
  speaker settings before delegating playback to `M5ToneOutputCore`.

The package intentionally does not preserve the old `SpeakerToneOutput` class or
`m5-buzzer-output` package name. The project is still young, and the current
users are our own showcases.

Validation targets:

```bash
pio test -d packages/monophonic-instrument -e native
pio run -d apps/plus2-buzzer-local-test
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
pio pkg pack packages/m5-tone-output --output /home/fcz/dev/m5stick/.tmp
pio run -d /tmp/m5-tone-output-consumer-reshape
pio run -d /tmp/m5-tone-output-archive-consumer-reshape
```

The first temporary consumer validates local `file://` package dependencies.
The package versions were bumped to `0.1.1` so archive consumers reinstall after
the shared waveform-sample headers moved into `monophonic-instrument` and the
buzzer output archive became `m5-tone-output`.

The second temporary consumer validates the generated
`monophonic-instrument-0.1.1.tar.gz` and `m5-tone-output-0.1.1.tar.gz` archives
that downstream showcase firmware uses.
