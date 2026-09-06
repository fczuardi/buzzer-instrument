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

void MonophonicInstrument::noteOn(uint8_t midiNoteNumber, uint8_t velocity) {
  if (velocity == 0) {
    noteOff(midiNoteNumber);
    return;
  }

  activeMidiNote_ = midiNoteNumber;
  noteActive_ = true;
}

void MonophonicInstrument::noteOff(uint8_t midiNoteNumber) {
  if (noteActive_ && activeMidiNote_ == midiNoteNumber) {
    noteActive_ = false;
  }
}

void MonophonicInstrument::stopAll() {
  noteActive_ = false;
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
