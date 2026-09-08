#include "MonophonicInstrument.h"

#include <cmath>

#include "MidiNote.h"

bool MonophonicInstrument::isNoteActive() const {
  return noteActive_;
}

uint8_t MonophonicInstrument::midiNoteNumber() const {
  return activeMidiNote_;
}

float MonophonicInstrument::frequencyHz() const {
  return bentFrequencyHz(midiNoteNumber());
}

int16_t MonophonicInstrument::pitchBendValue() const {
  return pitchBendValue_;
}

float MonophonicInstrument::pitchBendRangeSemitones() const {
  return pitchBendRangeSemitones_;
}

void MonophonicInstrument::noteName(char* output, size_t outputSize) const {
  midiNoteName(midiNoteNumber(), output, outputSize);
}

VoiceAction MonophonicInstrument::noteOn(
    uint8_t midiNoteNumber,
    uint8_t velocity) {
  if (velocity == 0) {
    return noteOff(midiNoteNumber);
  }

  const int existingIndex = heldNoteIndex(midiNoteNumber);
  if (existingIndex >= 0) {
    removeHeldNoteAt(static_cast<size_t>(existingIndex));
  }

  pushHeldNote(midiNoteNumber, velocity);
  activeMidiNote_ = midiNoteNumber;
  activeVelocity_ = velocity;
  noteActive_ = true;
  return startAction(midiNoteNumber, velocity);
}

VoiceAction MonophonicInstrument::noteOff(uint8_t midiNoteNumber) {
  const int existingIndex = heldNoteIndex(midiNoteNumber);
  if (existingIndex < 0) {
    return {VoiceActionType::None, 0, 0, 0.0f};
  }

  const bool removingActiveNote =
      noteActive_ && activeMidiNote_ == midiNoteNumber;
  removeHeldNoteAt(static_cast<size_t>(existingIndex));

  if (!removingActiveNote) {
    return {VoiceActionType::None, 0, 0, 0.0f};
  }

  if (heldNoteCount_ == 0) {
    noteActive_ = false;
    activeVelocity_ = 0;
    return stopAction(midiNoteNumber);
  }

  const HeldNote& previousNote = heldNotes_[heldNoteCount_ - 1];
  activeMidiNote_ = previousNote.midiNote;
  activeVelocity_ = previousNote.velocity;
  return startAction(activeMidiNote_, activeVelocity_);
}

VoiceAction MonophonicInstrument::handleNoteEvent(const NoteEvent& event) {
  switch (event.type) {
    case NoteEventType::NoteOn:
      return noteOn(event.note, event.velocity);
    case NoteEventType::NoteOff:
      return noteOff(event.note);
  }

  return {VoiceActionType::None, 0, 0, 0.0f};
}

VoiceAction MonophonicInstrument::handlePitchBendEvent(
    const PitchBendEvent& event) {
  pitchBendValue_ = normalizedPitchBend(event.value);

  if (!noteActive_) {
    return {VoiceActionType::None, 0, 0, 0.0f};
  }

  return startAction(activeMidiNote_, activeVelocity_);
}

VoiceAction MonophonicInstrument::stopAll() {
  pitchBendValue_ = 0;

  if (!noteActive_) {
    heldNoteCount_ = 0;
    activeVelocity_ = 0;
    return {VoiceActionType::None, 0, 0, 0.0f};
  }

  const uint8_t stoppedNote = activeMidiNote_;
  noteActive_ = false;
  activeVelocity_ = 0;
  heldNoteCount_ = 0;
  return stopAction(stoppedNote);
}

void MonophonicInstrument::setPitchBendRangeSemitones(float semitones) {
  pitchBendRangeSemitones_ = semitones < 0.0f ? 0.0f : semitones;
}

ToneWaveform MonophonicInstrument::waveform() const {
  return waveform_;
}

const char* MonophonicInstrument::waveformName() const {
  return toneWaveformName(waveform_);
}

void MonophonicInstrument::selectNextWaveform() {
  switch (waveform_) {
    case ToneWaveform::Square32:
      waveform_ = ToneWaveform::Saw32;
      break;
    case ToneWaveform::Saw32:
      waveform_ = ToneWaveform::Square32;
      break;
  }
}

VoiceAction MonophonicInstrument::startAction(
    uint8_t midiNoteNumber,
    uint8_t velocity) const {
  return {
      VoiceActionType::StartNote,
      midiNoteNumber,
      velocity,
      bentFrequencyHz(midiNoteNumber),
  };
}

VoiceAction MonophonicInstrument::stopAction(uint8_t midiNoteNumber) const {
  return {VoiceActionType::StopNote, midiNoteNumber, 0, 0.0f};
}

float MonophonicInstrument::bentFrequencyHz(uint8_t midiNoteNumber) const {
  const float baseFrequencyHz = midiNoteToFrequencyHz(midiNoteNumber);

  if (pitchBendValue_ == 0) {
    return baseFrequencyHz;
  }

  const float bendRatio =
      pitchBendValue_ > 0
          ? static_cast<float>(pitchBendValue_) / 8191.0f
          : static_cast<float>(pitchBendValue_) / 8192.0f;
  const float semitoneOffset = bendRatio * pitchBendRangeSemitones_;
  return baseFrequencyHz * std::pow(2.0f, semitoneOffset / 12.0f);
}

int16_t MonophonicInstrument::normalizedPitchBend(int16_t pitchBendValue) {
  return std::abs(pitchBendValue) <= PITCH_BEND_DEAD_ZONE
             ? 0
             : pitchBendValue;
}

int MonophonicInstrument::heldNoteIndex(uint8_t midiNoteNumber) const {
  for (size_t i = 0; i < heldNoteCount_; ++i) {
    if (heldNotes_[i].midiNote == midiNoteNumber) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

void MonophonicInstrument::removeHeldNoteAt(size_t index) {
  if (index >= heldNoteCount_) {
    return;
  }

  for (size_t i = index; i + 1 < heldNoteCount_; ++i) {
    heldNotes_[i] = heldNotes_[i + 1];
  }

  --heldNoteCount_;
}

void MonophonicInstrument::pushHeldNote(
    uint8_t midiNoteNumber,
    uint8_t velocity) {
  if (heldNoteCount_ == MAX_HELD_NOTES) {
    removeHeldNoteAt(0);
  }

  heldNotes_[heldNoteCount_].midiNote = midiNoteNumber;
  heldNotes_[heldNoteCount_].velocity = velocity;
  ++heldNoteCount_;
}
