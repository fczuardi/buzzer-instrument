#include "MonophonicInstrumentSink.h"

MonophonicInstrumentSink::MonophonicInstrumentSink(
    MonophonicInstrument& instrument,
    VoiceOutput& voiceOutput)
    : instrument_(instrument), voiceOutput_(voiceOutput) {
}

void MonophonicInstrumentSink::onNoteEvent(const NoteEvent& event) {
  applyVoiceAction(instrument_.handleNoteEvent(event));
}

void MonophonicInstrumentSink::onDisconnected() {
  applyVoiceAction(instrument_.stopAll());
}

void MonophonicInstrumentSink::applyVoiceAction(const VoiceAction& action) {
  switch (action.type) {
    case VoiceActionType::None:
      return;
    case VoiceActionType::StartNote:
      voiceOutput_.startNote(action.midiNote, instrument_.waveform());
      return;
    case VoiceActionType::StopNote:
      voiceOutput_.stopNote();
      return;
  }
}
