#pragma once

#include <cstddef>
#include <cstdint>

#include "ToneWaveform.h"

enum class VoiceActionType : uint8_t {
  None,
  StartNote,
  StopNote,
};

struct VoiceAction {
  VoiceActionType type;
  uint8_t midiNote;
};

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

  VoiceAction noteOn(uint8_t midiNoteNumber, uint8_t velocity);
  VoiceAction noteOff(uint8_t midiNoteNumber);
  VoiceAction stopAll();

  ToneWaveform waveform() const;
  const char* waveformName() const;
  void selectNextWaveform();

private:
  static constexpr size_t MAX_HELD_NOTES = 16;

  VoiceAction startAction(uint8_t midiNoteNumber) const;
  VoiceAction stopAction(uint8_t midiNoteNumber) const;
  int heldNoteIndex(uint8_t midiNoteNumber) const;
  void removeHeldNoteAt(size_t index);
  void pushHeldNote(uint8_t midiNoteNumber);

  bool noteActive_ = false;
  uint8_t activeMidiNote_ = DEFAULT_TEST_NOTE;
  uint8_t heldNotes_[MAX_HELD_NOTES] = {};
  size_t heldNoteCount_ = 0;
  ToneWaveform waveform_ = ToneWaveform::Saw32;
};
