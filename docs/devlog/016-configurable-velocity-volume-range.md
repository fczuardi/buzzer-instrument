# Slice 016: Configurable Velocity Volume Range

The first velocity mapping used a hardcoded `64..128` M5 speaker volume range.
That was a reasonable first default, but it made calibration awkward for the BLE
MIDI showcase: changing the range required editing the package, committing,
pushing, updating a SHA, and rebuilding the showcase.

This slice keeps the default but makes it configurable through
`SpeakerToneOutput`:

```cpp
speakerToneOutput.setVelocityVolumeRange(64, 128);
```

The mapping still belongs to the M5 speaker backend because the useful volume
range is hardware-specific. `MonophonicInstrument`, `VoiceAction`, and
`VoiceOutput` continue to carry velocity without knowing how this backend maps
it to physical sound.

If a caller passes the range in reverse order, the setter normalizes it so the
minimum is not greater than the maximum. This keeps experiments forgiving while
preserving a clear effective range.

The local smoke-test firmware defines `TEST_VELOCITY_VOLUME_RANGE` near the
other test constants. Changing that constant is now enough to try different
limits on the M5StickC Plus2.

Verification command:

```bash
env PLATFORMIO_CORE_DIR=.platformio-home pio test -e native
env PLATFORMIO_CORE_DIR=.platformio-home pio run
```

Expected result: behavior remains the same with the default `64..128` range,
but firmware code can override it before notes are played.
