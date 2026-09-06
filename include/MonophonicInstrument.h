#pragma once

#include <cstddef>
#include <cstdint>

#include "ToneWaveform.h"

// Owns the musical state for the current one-note instrument experiment.
//
// This class deliberately has no Arduino or M5 dependency. It accepts MIDI-like
// note events and decides which single note should be active; hardware code
// decides how to render that choice.
class MonophonicInstrument {
public:
  static constexpr uint8_t DEFAULT_TEST_NOTE = 60;

  bool isNoteActive() const;
  uint8_t midiNoteNumber() const;
  float frequencyHz() const;
  void noteName(char* output, size_t outputSize) const;

  void noteOn(uint8_t midiNoteNumber, uint8_t velocity);
  void noteOff(uint8_t midiNoteNumber);
  void stopAll();

  ToneWaveform waveform() const;
  const char* waveformName() const;
  void selectNextWaveform();

private:
  bool noteActive_ = false;
  uint8_t activeMidiNote_ = DEFAULT_TEST_NOTE;
  ToneWaveform waveform_ = ToneWaveform::Saw32;
};
