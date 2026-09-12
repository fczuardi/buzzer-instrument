#pragma once

#include "InstrumentEventSink.h"
#include "MonophonicInstrument.h"
#include "VoiceOutput.h"

// Applies incoming instrument events to the current monophonic voice.
//
// This class owns no state by itself. The caller must keep the referenced
// instrument and output alive for at least as long as this sink.
class MonophonicInstrumentSink : public InstrumentEventSink {
public:
  MonophonicInstrumentSink(
      MonophonicInstrument& instrument,
      VoiceOutput& voiceOutput);

  void onNoteEvent(const NoteEvent& event) override;
  void onPitchBendEvent(const PitchBendEvent& event) override;
  void onDisconnected() override;

  uint8_t activeMidiChannel() const;

  // Silence all held note state and physical output immediately.
  //
  // This is useful for local panic controls. It is intentionally separate from
  // onDisconnected() because a panic action does not imply a transport event.
  void panic();

private:
  void applyVoiceAction(const VoiceAction& action);

  MonophonicInstrument& instrument_;
  VoiceOutput& voiceOutput_;
};
