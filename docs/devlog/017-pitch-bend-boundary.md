# Slice 017: Pitch Bend Boundary

The shared firmware contract now includes `PitchBendEvent`, so the buzzer
instrument package must accept that event through `InstrumentEventSink`.

This slice updates `MonophonicInstrumentSink` with:

```cpp
void onPitchBendEvent(const PitchBendEvent& event) override;
```

The current implementation intentionally does nothing. That is still useful:
the BLE input and the instrument package can now compile against the same typed
boundary, while the musical choice of how much pitch bend affects frequency is
left for a later slice.

The native test checks that a pitch bend event can arrive while a note is
playing without stopping or restarting the current voice. The next musical step
can store the latest bend value and apply a range such as +/-2 semitones to the
active note frequency.
