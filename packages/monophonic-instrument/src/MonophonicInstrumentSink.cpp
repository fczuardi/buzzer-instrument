#include "MonophonicInstrumentSink.h"

MonophonicInstrumentSink::MonophonicInstrumentSink(
    MonophonicInstrument& instrument,
    VoiceOutput& voiceOutput)
    : instrument_(instrument), voiceOutput_(voiceOutput) {
}

void MonophonicInstrumentSink::onNoteEvent(const NoteEvent& event) {
  applyVoiceAction(instrument_.handleNoteEvent(event));
}

void MonophonicInstrumentSink::onPitchBendEvent(const PitchBendEvent& event) {
  applyVoiceAction(instrument_.handlePitchBendEvent(event));
}

void MonophonicInstrumentSink::onDisconnected() {
  panic();
}

uint8_t MonophonicInstrumentSink::activeMidiChannel() const {
  return instrument_.activeMidiChannel();
}

void MonophonicInstrumentSink::panic() {
  instrument_.stopAll();
  voiceOutput_.stopNote();
}

void MonophonicInstrumentSink::applyVoiceAction(const VoiceAction& action) {
  switch (action.type) {
    case VoiceActionType::None:
      return;
    case VoiceActionType::StartNote:
      voiceOutput_.startNote(
          action.midiNote,
          action.frequencyHz,
          instrument_.waveform(),
          action.velocity);
      return;
    case VoiceActionType::StopNote:
      voiceOutput_.stopNote();
      return;
  }
}
