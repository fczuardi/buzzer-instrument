#include "MonophonicInstrument.h"

#include <cmath>

#include "MidiNote.h"

bool MonophonicInstrument::isNoteActive() const {
  return notePriority_.isNoteActive();
}

uint8_t MonophonicInstrument::activeMidiChannel() const {
  return notePriority_.activeMidiChannel();
}

uint8_t MonophonicInstrument::midiNoteNumber() const {
  return notePriority_.activeMidiNote();
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
  return voiceActionFromNoteAction(
      notePriority_.noteOn(midiNoteNumber, velocity));
}

VoiceAction MonophonicInstrument::noteOff(uint8_t midiNoteNumber) {
  return voiceActionFromNoteAction(notePriority_.noteOff(midiNoteNumber));
}

VoiceAction MonophonicInstrument::handleNoteEvent(const NoteEvent& event) {
  return voiceActionFromNoteAction(notePriority_.handleNoteEvent(event));
}

VoiceAction MonophonicInstrument::handlePitchBendEvent(
    const PitchBendEvent& event) {
  pitchBendValue_ = normalizedPitchBend(event.value);

  if (!notePriority_.isNoteActive()) {
    return {VoiceActionType::None, 0, 0, 0.0f};
  }

  return startAction(
      notePriority_.activeMidiNote(),
      notePriority_.activeVelocity());
}

VoiceAction MonophonicInstrument::stopAll() {
  pitchBendValue_ = 0;
  return voiceActionFromNoteAction(notePriority_.stopAll());
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

VoiceAction MonophonicInstrument::voiceActionFromNoteAction(
    const MonophonicNoteAction& action) const {
  switch (action.type) {
    case MonophonicNoteActionType::None:
      return {VoiceActionType::None, 0, 0, 0.0f};
    case MonophonicNoteActionType::StartNote:
      return startAction(action.midiNote, action.velocity);
    case MonophonicNoteActionType::StopNote:
      return stopAction(action.midiNote);
  }

  return {VoiceActionType::None, 0, 0, 0.0f};
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
