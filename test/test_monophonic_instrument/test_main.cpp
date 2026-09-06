#include <unity.h>

#include "MonophonicInstrument.h"

void assertVoiceAction(
    const VoiceAction& action,
    VoiceActionType expectedType,
    uint8_t expectedMidiNote) {
  TEST_ASSERT_EQUAL(expectedType, action.type);
  TEST_ASSERT_EQUAL_UINT8(expectedMidiNote, action.midiNote);
}

void test_starts_idle_on_default_c4_with_saw_waveform() {
  MonophonicInstrument instrument;
  char noteName[5];

  instrument.noteName(noteName, sizeof(noteName));

  TEST_ASSERT_FALSE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(60, instrument.midiNoteNumber());
  TEST_ASSERT_EQUAL_STRING("C4", noteName);
  TEST_ASSERT_EQUAL(ToneWaveform::Saw32, instrument.waveform());
  TEST_ASSERT_EQUAL_STRING("saw32", instrument.waveformName());
}

void test_note_on_activates_requested_note() {
  MonophonicInstrument instrument;
  char noteName[5];

  const VoiceAction action = instrument.noteOn(64, 96);
  instrument.noteName(noteName, sizeof(noteName));

  assertVoiceAction(action, VoiceActionType::StartNote, 64);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, instrument.midiNoteNumber());
  TEST_ASSERT_EQUAL_STRING("E4", noteName);
}

void test_latest_note_on_replaces_active_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 90);
  const VoiceAction action = instrument.noteOn(67, 80);

  assertVoiceAction(action, VoiceActionType::StartNote, 67);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(67, instrument.midiNoteNumber());
}

void test_releasing_current_note_returns_to_previous_held_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.noteOn(64, 100);
  const VoiceAction action = instrument.noteOff(64);

  assertVoiceAction(action, VoiceActionType::StartNote, 60);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(60, instrument.midiNoteNumber());
}

void test_matching_note_off_stops_when_no_previous_note_is_held() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.noteOff(60);

  assertVoiceAction(action, VoiceActionType::StopNote, 60);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(60, instrument.midiNoteNumber());
}

void test_note_off_for_non_current_note_does_not_stop_voice() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.noteOn(64, 100);
  const VoiceAction action = instrument.noteOff(60);

  assertVoiceAction(action, VoiceActionType::None, 0);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, instrument.midiNoteNumber());
}

void test_note_off_for_unheld_note_is_ignored() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.noteOff(62);

  assertVoiceAction(action, VoiceActionType::None, 0);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(60, instrument.midiNoteNumber());
}

void test_repeated_note_on_does_not_leave_duplicate_held_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.noteOn(64, 100);
  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.noteOff(60);

  assertVoiceAction(action, VoiceActionType::StartNote, 64);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, instrument.midiNoteNumber());
}

void test_midi_note_zero_is_valid() {
  MonophonicInstrument instrument;
  char noteName[5];

  const VoiceAction startAction = instrument.noteOn(0, 100);
  instrument.noteName(noteName, sizeof(noteName));

  assertVoiceAction(startAction, VoiceActionType::StartNote, 0);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(0, instrument.midiNoteNumber());
  TEST_ASSERT_EQUAL_STRING("C-1", noteName);

  const VoiceAction stopAction = instrument.noteOff(0);
  assertVoiceAction(stopAction, VoiceActionType::StopNote, 0);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
}

void test_note_on_with_zero_velocity_is_note_off() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.noteOn(60, 0);

  assertVoiceAction(action, VoiceActionType::StopNote, 60);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
}

void test_stop_all_silences_active_note_and_clears_held_notes() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.noteOn(72, 100);
  const VoiceAction stopAllAction = instrument.stopAll();

  assertVoiceAction(stopAllAction, VoiceActionType::StopNote, 72);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(72, instrument.midiNoteNumber());

  const VoiceAction noteOffAction = instrument.noteOff(60);
  assertVoiceAction(noteOffAction, VoiceActionType::None, 0);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
}

void test_frequency_and_name_follow_active_note() {
  MonophonicInstrument instrument;
  char noteName[5];

  instrument.noteOn(69, 100);
  instrument.noteName(noteName, sizeof(noteName));

  TEST_ASSERT_EQUAL_STRING("A4", noteName);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 440.0f, instrument.frequencyHz());
}

void test_waveform_toggle_alternates_between_saw_and_square() {
  MonophonicInstrument instrument;

  TEST_ASSERT_EQUAL(ToneWaveform::Saw32, instrument.waveform());

  instrument.selectNextWaveform();
  TEST_ASSERT_EQUAL(ToneWaveform::Square32, instrument.waveform());
  TEST_ASSERT_EQUAL_STRING("sq32", instrument.waveformName());

  instrument.selectNextWaveform();
  TEST_ASSERT_EQUAL(ToneWaveform::Saw32, instrument.waveform());
  TEST_ASSERT_EQUAL_STRING("saw32", instrument.waveformName());
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_starts_idle_on_default_c4_with_saw_waveform);
  RUN_TEST(test_note_on_activates_requested_note);
  RUN_TEST(test_latest_note_on_replaces_active_note);
  RUN_TEST(test_releasing_current_note_returns_to_previous_held_note);
  RUN_TEST(test_matching_note_off_stops_when_no_previous_note_is_held);
  RUN_TEST(test_note_off_for_non_current_note_does_not_stop_voice);
  RUN_TEST(test_note_off_for_unheld_note_is_ignored);
  RUN_TEST(test_repeated_note_on_does_not_leave_duplicate_held_note);
  RUN_TEST(test_midi_note_zero_is_valid);
  RUN_TEST(test_note_on_with_zero_velocity_is_note_off);
  RUN_TEST(test_stop_all_silences_active_note_and_clears_held_notes);
  RUN_TEST(test_frequency_and_name_follow_active_note);
  RUN_TEST(test_waveform_toggle_alternates_between_saw_and_square);
  return UNITY_END();
}
