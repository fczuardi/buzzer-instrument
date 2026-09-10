#pragma once

#include <cstddef>
#include <cstdint>

#include "NoteEvent.h"

enum class MonophonicNoteActionType : uint8_t {
  None,
  StartNote,
  StopNote,
};

struct MonophonicNoteAction {
  MonophonicNoteActionType type;
  uint8_t midiNote;
  uint8_t velocity;
};

// Tracks held MIDI notes and chooses the one monophonic note that should sound.
//
// This is the reusable policy shared by simple tone instruments and synth
// backends. It deliberately does not know about frequency, waveform, pitch
// bend, AMY, M5Unified, or physical output.
class MonophonicNotePriority {
 public:
  static constexpr uint8_t DEFAULT_TEST_NOTE = 60;

  bool isNoteActive() const;
  uint8_t activeMidiNote() const;
  uint8_t activeVelocity() const;

  MonophonicNoteAction noteOn(uint8_t midiNote, uint8_t velocity);
  MonophonicNoteAction noteOff(uint8_t midiNote);
  MonophonicNoteAction handleNoteEvent(const NoteEvent& event);
  MonophonicNoteAction stopAll();

 private:
  // Fixed capacity keeps the event path allocation-free. If more notes are
  // held, the oldest held note is discarded and recent priority is preserved.
  static constexpr size_t MAX_HELD_NOTES = 16;

  struct HeldNote {
    uint8_t midiNote = 0;
    uint8_t velocity = 0;
  };

  static MonophonicNoteAction startAction(
      uint8_t midiNote,
      uint8_t velocity);
  static MonophonicNoteAction stopAction(uint8_t midiNote);

  int heldNoteIndex(uint8_t midiNote) const;
  void removeHeldNoteAt(size_t index);
  void pushHeldNote(uint8_t midiNote, uint8_t velocity);

  bool noteActive_ = false;
  uint8_t activeMidiNote_ = DEFAULT_TEST_NOTE;
  uint8_t activeVelocity_ = 0;
  HeldNote heldNotes_[MAX_HELD_NOTES] = {};
  size_t heldNoteCount_ = 0;
};
