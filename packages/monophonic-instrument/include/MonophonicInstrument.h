#pragma once

#include <cstdint>

#include "NoteEvent.h"
#include "MonophonicNotePriority.h"
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
  uint8_t midiChannel = 0;
};

// Owns the musical state for the current one-note instrument experiment.
//
// This class deliberately has no Arduino or M5 dependency. It accepts MIDI-like
// note events and decides which single note should be active; hardware code
// decides how to render that choice.
class MonophonicInstrument {
public:
  static constexpr uint8_t DEFAULT_TEST_NOTE =
      MonophonicNotePriority::DEFAULT_TEST_NOTE;

  bool isNoteActive() const;
  uint8_t activeMidiChannel() const;
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
  static constexpr int16_t PITCH_BEND_DEAD_ZONE = 128;
  static constexpr float DEFAULT_PITCH_BEND_RANGE_SEMITONES = 2.0f;

  VoiceAction startAction(
      uint8_t midiChannel,
      uint8_t midiNoteNumber,
      uint8_t velocity) const;
  VoiceAction stopAction(uint8_t midiChannel, uint8_t midiNoteNumber) const;
  VoiceAction voiceActionFromNoteAction(
      const MonophonicNoteAction& action) const;
  float bentFrequencyHz(uint8_t midiNoteNumber) const;
  static int16_t normalizedPitchBend(int16_t pitchBendValue);

  int16_t pitchBendValue_ = 0;
  float pitchBendRangeSemitones_ = DEFAULT_PITCH_BEND_RANGE_SEMITONES;
  ToneWaveform waveform_ = ToneWaveform::Saw32;
  MonophonicNotePriority notePriority_;
};
