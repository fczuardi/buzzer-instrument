#pragma once

#include <cstddef>
#include <cstdint>

#include "NoteEvent.h"
#include "PitchBendEvent.h"
#include "ToneWaveform.h"

enum class VoiceActionType : uint8_t {
  None,
  StartNote,
  StopNote,
};

struct VoiceAction {
  VoiceActionType type;
  uint8_t midiNote;
  uint8_t velocity;
  float frequencyHz;
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
  int16_t pitchBendValue() const;
  float pitchBendRangeSemitones() const;
  void noteName(char* output, size_t outputSize) const;

  VoiceAction noteOn(uint8_t midiNoteNumber, uint8_t velocity);
  VoiceAction noteOff(uint8_t midiNoteNumber);
  VoiceAction handleNoteEvent(const NoteEvent& event);
  VoiceAction handlePitchBendEvent(const PitchBendEvent& event);
  VoiceAction stopAll();
  void setPitchBendRangeSemitones(float semitones);

  ToneWaveform waveform() const;
  const char* waveformName() const;
  void selectNextWaveform();

private:
  // Fixed capacity keeps the event path allocation-free. If more notes are
  // held, the oldest held note is discarded and recent priority is preserved.
  static constexpr size_t MAX_HELD_NOTES = 16;
  static constexpr int16_t PITCH_BEND_DEAD_ZONE = 128;
  static constexpr float DEFAULT_PITCH_BEND_RANGE_SEMITONES = 2.0f;

  struct HeldNote {
    uint8_t midiNote = 0;
    uint8_t velocity = 0;
  };

  VoiceAction startAction(uint8_t midiNoteNumber, uint8_t velocity) const;
  VoiceAction stopAction(uint8_t midiNoteNumber) const;
  float bentFrequencyHz(uint8_t midiNoteNumber) const;
  static int16_t normalizedPitchBend(int16_t pitchBendValue);
  int heldNoteIndex(uint8_t midiNoteNumber) const;
  void removeHeldNoteAt(size_t index);
  void pushHeldNote(uint8_t midiNoteNumber, uint8_t velocity);

  bool noteActive_ = false;
  uint8_t activeMidiNote_ = DEFAULT_TEST_NOTE;
  uint8_t activeVelocity_ = 0;
  int16_t pitchBendValue_ = 0;
  float pitchBendRangeSemitones_ = DEFAULT_PITCH_BEND_RANGE_SEMITONES;
  HeldNote heldNotes_[MAX_HELD_NOTES] = {};
  size_t heldNoteCount_ = 0;
  ToneWaveform waveform_ = ToneWaveform::Saw32;
};
