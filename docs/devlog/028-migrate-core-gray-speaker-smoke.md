# Slice 028: Migrate Core Gray Speaker Smoke Test

The old `m5-speaker-instrument` repo had already proven the M5Stack Core Gray
speaker path with a local note sweep. The goal here is to move that useful
behavior into the unified `monophonic-instrument` home instead of creating a
new independent experiment.

The duplicated old `M5SpeakerToneOutput` implementation is not copied over.
Instead, `packages/m5-tone-output` now exposes `M5CoreGrayToneOutput`, a small
wrapper that carries the Core Gray defaults validated earlier:

- speaker volume `35`
- velocity-to-volume range `1..35`
- default waveform `Square32`

Tone playback, waveform samples, volume state, `VoiceOutput`, and
velocity-to-volume mapping all stay in `M5ToneOutputCore`. While adding the
Gray wrapper, `M5BuzzerToneOutput` was simplified to inherit from
`M5ToneOutputCore` too, leaving only its Plus2 buzzer speaker configuration in
the wrapper.

The migrated local app lives at `apps/core-gray-speaker-local-test`. It keeps
the old hardware interaction:

- Button A starts or stops the selected tone.
- Button B advances through C3, C4, A4, C5, and C6.
- Button C toggles between `sq32` and `saw32`.

The app now uses shared `MidiNote` helpers for names and frequencies instead of
hardcoded frequency tables.

The `m5-tone-output` package version was bumped to `0.1.2` because this slice
adds the public `M5CoreGrayToneOutput` header.

Validation targets:

```bash
pio test -d packages/monophonic-instrument -e native
pio run -d ci/consumers/m5-tone-output
pio run -d apps/plus2-buzzer-local-test
pio run -d apps/core-gray-speaker-local-test
pio pkg pack packages/m5-tone-output --output /home/fcz/dev/m5stick/.tmp
```

Hardware target:

```bash
pio run -d apps/core-gray-speaker-local-test --target upload
pio device monitor -d apps/core-gray-speaker-local-test
```
