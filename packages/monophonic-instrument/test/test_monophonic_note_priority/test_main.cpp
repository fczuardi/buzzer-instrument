#include <unity.h>

#include "MonophonicNotePriority.h"

void assertNoteAction(
    const MonophonicNoteAction& action,
    MonophonicNoteActionType expectedType,
    uint8_t expectedMidiNote,
    uint8_t expectedVelocity = 0) {
  TEST_ASSERT_EQUAL(expectedType, action.type);
  TEST_ASSERT_EQUAL_UINT8(expectedMidiNote, action.midiNote);
  TEST_ASSERT_EQUAL_UINT8(expectedVelocity, action.velocity);
}

void test_note_on_activates_requested_note() {
  MonophonicNotePriority priority;

  const MonophonicNoteAction action = priority.noteOn(64, 96);

  assertNoteAction(action, MonophonicNoteActionType::StartNote, 64, 96);
  TEST_ASSERT_TRUE(priority.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, priority.activeMidiNote());
  TEST_ASSERT_EQUAL_UINT8(96, priority.activeVelocity());
}

void test_releasing_current_note_returns_to_previous_held_note() {
  MonophonicNotePriority priority;

  priority.noteOn(60, 72);
  priority.noteOn(64, 110);
  const MonophonicNoteAction action = priority.noteOff(64);

  assertNoteAction(action, MonophonicNoteActionType::StartNote, 60, 72);
  TEST_ASSERT_TRUE(priority.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(60, priority.activeMidiNote());
  TEST_ASSERT_EQUAL_UINT8(72, priority.activeVelocity());
}

void test_note_off_for_non_current_note_does_not_touch_active_note() {
  MonophonicNotePriority priority;

  priority.noteOn(60, 100);
  priority.noteOn(64, 90);
  const MonophonicNoteAction action = priority.noteOff(60);

  assertNoteAction(action, MonophonicNoteActionType::None, 0);
  TEST_ASSERT_TRUE(priority.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, priority.activeMidiNote());
}

void test_note_on_with_zero_velocity_is_note_off() {
  MonophonicNotePriority priority;

  priority.noteOn(60, 100);
  const MonophonicNoteAction action = priority.noteOn(60, 0);

  assertNoteAction(action, MonophonicNoteActionType::StopNote, 60);
  TEST_ASSERT_FALSE(priority.isNoteActive());
}

void test_stop_all_clears_held_notes() {
  MonophonicNotePriority priority;

  priority.noteOn(60, 100);
  priority.noteOn(64, 100);
  const MonophonicNoteAction action = priority.stopAll();

  assertNoteAction(action, MonophonicNoteActionType::StopNote, 64);
  TEST_ASSERT_FALSE(priority.isNoteActive());

  const MonophonicNoteAction staleOff = priority.noteOff(60);
  assertNoteAction(staleOff, MonophonicNoteActionType::None, 0);
}

void test_cross_channel_fallback_preserves_note_identity() {
  MonophonicNotePriority priority;

  priority.noteOn(0, 60, 72);
  priority.noteOn(1, 64, 110);
  const MonophonicNoteAction fallback = priority.noteOff(1, 64);

  assertNoteAction(fallback, MonophonicNoteActionType::StartNote, 60, 72);
  TEST_ASSERT_EQUAL_UINT8(0, priority.activeMidiChannel());

  const MonophonicNoteAction stop = priority.noteOff(0, 60);
  assertNoteAction(stop, MonophonicNoteActionType::StopNote, 60);
  TEST_ASSERT_FALSE(priority.isNoteActive());
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_note_on_activates_requested_note);
  RUN_TEST(test_releasing_current_note_returns_to_previous_held_note);
  RUN_TEST(test_note_off_for_non_current_note_does_not_touch_active_note);
  RUN_TEST(test_note_on_with_zero_velocity_is_note_off);
  RUN_TEST(test_stop_all_clears_held_notes);
  RUN_TEST(test_cross_channel_fallback_preserves_note_identity);
  return UNITY_END();
}
