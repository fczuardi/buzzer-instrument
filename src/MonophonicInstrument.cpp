#include "MonophonicInstrument.h"

#include "MidiNote.h"

bool MonophonicInstrument::isNoteActive() const {
  return noteActive_;
}

uint8_t MonophonicInstrument::midiNoteNumber() const {
  return activeMidiNote_;
}

float MonophonicInstrument::frequencyHz() const {
  return midiNoteToFrequencyHz(midiNoteNumber());
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

  pushHeldNote(midiNoteNumber);
  activeMidiNote_ = midiNoteNumber;
  noteActive_ = true;
  return startAction(midiNoteNumber);
}

VoiceAction MonophonicInstrument::noteOff(uint8_t midiNoteNumber) {
  const int existingIndex = heldNoteIndex(midiNoteNumber);
  if (existingIndex < 0) {
    return {VoiceActionType::None, 0};
  }

  const bool removingActiveNote =
      noteActive_ && activeMidiNote_ == midiNoteNumber;
  removeHeldNoteAt(static_cast<size_t>(existingIndex));

  if (!removingActiveNote) {
    return {VoiceActionType::None, 0};
  }

  if (heldNoteCount_ == 0) {
    noteActive_ = false;
    return stopAction(midiNoteNumber);
  }

  activeMidiNote_ = heldNotes_[heldNoteCount_ - 1];
  return startAction(activeMidiNote_);
}

VoiceAction MonophonicInstrument::handleNoteEvent(const NoteEvent& event) {
  switch (event.type) {
    case NoteEventType::NoteOn:
      return noteOn(event.note, event.velocity);
    case NoteEventType::NoteOff:
      return noteOff(event.note);
  }

  return {VoiceActionType::None, 0};
}

VoiceAction MonophonicInstrument::stopAll() {
  if (!noteActive_) {
    heldNoteCount_ = 0;
    return {VoiceActionType::None, 0};
  }

  const uint8_t stoppedNote = activeMidiNote_;
  noteActive_ = false;
  heldNoteCount_ = 0;
  return stopAction(stoppedNote);
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

VoiceAction MonophonicInstrument::startAction(uint8_t midiNoteNumber) const {
  return {VoiceActionType::StartNote, midiNoteNumber};
}

VoiceAction MonophonicInstrument::stopAction(uint8_t midiNoteNumber) const {
  return {VoiceActionType::StopNote, midiNoteNumber};
}

int MonophonicInstrument::heldNoteIndex(uint8_t midiNoteNumber) const {
  for (size_t i = 0; i < heldNoteCount_; ++i) {
    if (heldNotes_[i] == midiNoteNumber) {
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

void MonophonicInstrument::pushHeldNote(uint8_t midiNoteNumber) {
  if (heldNoteCount_ == MAX_HELD_NOTES) {
    removeHeldNoteAt(0);
  }

  heldNotes_[heldNoteCount_] = midiNoteNumber;
  ++heldNoteCount_;
}
