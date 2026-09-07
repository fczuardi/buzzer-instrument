# Slice 019: Configurable Pitch Bend Range

Audible pitch bend is useful, but the best musical range is not necessarily the
package default. A two-semitone bend is conventional for many instruments, while
showcases may want wider or narrower movement while testing controllers.

This slice adds an explicit setup API to `MonophonicInstrument`:

```cpp
instrument.setPitchBendRangeSemitones(12.0f);
```

The default remains plus or minus two semitones. Negative ranges are clamped to
zero, which gives firmware a simple way to disable audible bend while still
accepting `PitchBendEvent`.

The mapping remains instrument policy. `SpeakerToneOutput` still receives a
plain frequency and does not need to understand MIDI pitch bend.

Native tests cover a custom one-octave range and the negative-range clamp.

The slice also adds a clearer volume calibration overload:

```cpp
speakerToneOutput.setVelocityVolumeRange(64, 128);
```

The older `VelocityVolumeRange` overload remains available, but showcase code
can now configure pitch bend and velocity volume with similarly direct setup
calls.
