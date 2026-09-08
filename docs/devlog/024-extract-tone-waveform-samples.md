# Slice 024: Extract Tone Waveform Samples

Both current M5 output implementations carried identical 32-sample square and
saw wave buffers. That duplication was useful while the Core Gray speaker path
was still exploratory, but it is now stable enough to share before extracting
the Core Gray output package.

This slice adds `ToneWaveformSamples` beside `ToneWaveform` in the shared
`monophonic-instrument` package. The helper is pure waveform data lookup: it
does not include M5Unified and does not call `M5.Speaker`. Hardware output
classes still own board setup, speaker channel use, velocity mapping, and the
actual tone call.

`SpeakerToneOutput` in the legacy root package and in `packages/m5-buzzer-output`
now use `toneWaveformSamples()` instead of carrying local copies of the sample
tables. This prepares `m5-speaker-output` to reuse the same samples while
keeping its Core Gray calibration separate.

Native tests cover the square and saw sample counts and boundary values so the
helper remains a visible contract rather than an incidental implementation
detail.

Validation targets:

```bash
pio test -e native
pio test -d packages/monophonic-instrument -e native
pio run
pio pkg pack packages/monophonic-instrument --output /home/fcz/dev/m5stick/.tmp
pio pkg pack packages/m5-buzzer-output --output /home/fcz/dev/m5stick/.tmp
```

Verified result:

- Root native tests passed with 44/44 test cases.
- `packages/monophonic-instrument` native tests passed with 44/44 test cases.
- The M5StickC Plus2 root firmware build passed and compiled
  `ToneWaveformSamples.cpp`.
- The legacy root package, `monophonic-instrument`, and `m5-buzzer-output`
  package archives packed successfully.
- A temporary Arduino consumer built successfully from local package paths and
  explicitly included `ToneWaveformSamples.h`.
- A temporary Arduino consumer built successfully from packed archives and
  compiled `ToneWaveformSamples.cpp` from `monophonic-instrument` while linking
  `m5-buzzer-output`.
- The existing BLE MIDI buzzer showcase build still passed against the refreshed
  local archives.
