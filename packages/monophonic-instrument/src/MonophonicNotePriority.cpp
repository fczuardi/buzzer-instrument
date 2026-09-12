#include "MonophonicNotePriority.h"

bool MonophonicNotePriority::isNoteActive() const {
  return noteActive_;
}

uint8_t MonophonicNotePriority::activeMidiChannel() const {
  return activeMidiChannel_;
}

uint8_t MonophonicNotePriority::activeMidiNote() const {
  return activeMidiNote_;
}

uint8_t MonophonicNotePriority::activeVelocity() const {
  return activeVelocity_;
}

MonophonicNoteAction MonophonicNotePriority::noteOn(
    uint8_t midiNote,
    uint8_t velocity) {
  return noteOn(0, midiNote, velocity);
}

MonophonicNoteAction MonophonicNotePriority::noteOn(
    uint8_t midiChannel,
    uint8_t midiNote,
    uint8_t velocity) {
  if (velocity == 0) {
    return noteOff(midiChannel, midiNote);
  }

  const int existingIndex = heldNoteIndex(midiChannel, midiNote);
  if (existingIndex >= 0) {
    removeHeldNoteAt(static_cast<size_t>(existingIndex));
  }

  pushHeldNote(midiChannel, midiNote, velocity);
  activeMidiChannel_ = midiChannel;
  activeMidiNote_ = midiNote;
  activeVelocity_ = velocity;
  noteActive_ = true;
  return startAction(midiNote, velocity);
}

MonophonicNoteAction MonophonicNotePriority::noteOff(uint8_t midiNote) {
  return noteOff(0, midiNote);
}

MonophonicNoteAction MonophonicNotePriority::noteOff(
    uint8_t midiChannel,
    uint8_t midiNote) {
  const int existingIndex = heldNoteIndex(midiChannel, midiNote);
  if (existingIndex < 0) {
    return {MonophonicNoteActionType::None, 0, 0};
  }

  const bool removingActiveNote = noteActive_ &&
      activeMidiChannel_ == midiChannel && activeMidiNote_ == midiNote;
  removeHeldNoteAt(static_cast<size_t>(existingIndex));

  if (!removingActiveNote) {
    return {MonophonicNoteActionType::None, 0, 0};
  }

  if (heldNoteCount_ == 0) {
    noteActive_ = false;
    activeMidiChannel_ = 0;
    activeVelocity_ = 0;
    return stopAction(midiNote);
  }

  const HeldNote& previousNote = heldNotes_[heldNoteCount_ - 1];
  activeMidiChannel_ = previousNote.midiChannel;
  activeMidiNote_ = previousNote.midiNote;
  activeVelocity_ = previousNote.velocity;
  return startAction(activeMidiNote_, activeVelocity_);
}

MonophonicNoteAction MonophonicNotePriority::handleNoteEvent(
    const NoteEvent& event) {
  switch (event.type) {
    case NoteEventType::NoteOn:
      return noteOn(event.channel, event.note, event.velocity);
    case NoteEventType::NoteOff:
      return noteOff(event.channel, event.note);
  }

  return {MonophonicNoteActionType::None, 0, 0};
}

MonophonicNoteAction MonophonicNotePriority::stopAll() {
  if (!noteActive_) {
    activeVelocity_ = 0;
    activeMidiChannel_ = 0;
    heldNoteCount_ = 0;
    return {MonophonicNoteActionType::None, 0, 0};
  }

  const uint8_t stoppedNote = activeMidiNote_;
  noteActive_ = false;
  activeMidiChannel_ = 0;
  activeVelocity_ = 0;
  heldNoteCount_ = 0;
  return stopAction(stoppedNote);
}

MonophonicNoteAction MonophonicNotePriority::startAction(
    uint8_t midiNote,
    uint8_t velocity) {
  return {MonophonicNoteActionType::StartNote, midiNote, velocity};
}

MonophonicNoteAction MonophonicNotePriority::stopAction(uint8_t midiNote) {
  return {MonophonicNoteActionType::StopNote, midiNote, 0};
}

int MonophonicNotePriority::heldNoteIndex(
    uint8_t midiChannel,
    uint8_t midiNote) const {
  for (size_t i = 0; i < heldNoteCount_; ++i) {
    if (heldNotes_[i].midiChannel == midiChannel &&
        heldNotes_[i].midiNote == midiNote) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

void MonophonicNotePriority::removeHeldNoteAt(size_t index) {
  if (index >= heldNoteCount_) {
    return;
  }

  for (size_t i = index; i + 1 < heldNoteCount_; ++i) {
    heldNotes_[i] = heldNotes_[i + 1];
  }

  --heldNoteCount_;
}

void MonophonicNotePriority::pushHeldNote(
    uint8_t midiChannel,
    uint8_t midiNote,
    uint8_t velocity) {
  if (heldNoteCount_ == MAX_HELD_NOTES) {
    removeHeldNoteAt(0);
  }

  heldNotes_[heldNoteCount_].midiChannel = midiChannel;
  heldNotes_[heldNoteCount_].midiNote = midiNote;
  heldNotes_[heldNoteCount_].velocity = velocity;
  ++heldNoteCount_;
}
