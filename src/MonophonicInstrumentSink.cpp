#include "MonophonicInstrumentSink.h"

MonophonicInstrumentSink::MonophonicInstrumentSink(
    MonophonicInstrument& instrument,
    VoiceOutput& voiceOutput)
    : instrument_(instrument), voiceOutput_(voiceOutput) {
}

void MonophonicInstrumentSink::onNoteEvent(const NoteEvent& event) {
  applyVoiceAction(instrument_.handleNoteEvent(event));
}

void MonophonicInstrumentSink::onPitchBendEvent(const PitchBendEvent&) {
  // Pitch bend is part of the shared instrument boundary, but this first
  // monophonic buzzer implementation does not apply it to frequency yet.
}

void MonophonicInstrumentSink::onDisconnected() {
  panic();
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
          instrument_.waveform(),
          action.velocity);
      return;
    case VoiceActionType::StopNote:
      voiceOutput_.stopNote();
      return;
  }
}
