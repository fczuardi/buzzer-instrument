#include <unity.h>

#include "MidiNote.h"
#include "MonophonicInstrument.h"

void assertVoiceAction(
    const VoiceAction& action,
    VoiceActionType expectedType,
    uint8_t expectedMidiNote,
    uint8_t expectedVelocity = 0) {
  TEST_ASSERT_EQUAL(expectedType, action.type);
  TEST_ASSERT_EQUAL_UINT8(expectedMidiNote, action.midiNote);
  TEST_ASSERT_EQUAL_UINT8(expectedVelocity, action.velocity);
}

void assertStartFrequency(
    const VoiceAction& action,
    float expectedFrequencyHz) {
  TEST_ASSERT_EQUAL(VoiceActionType::StartNote, action.type);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, expectedFrequencyHz, action.frequencyHz);
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

  assertVoiceAction(action, VoiceActionType::StartNote, 64, 96);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, instrument.midiNoteNumber());
  TEST_ASSERT_EQUAL_STRING("E4", noteName);
}

void test_latest_note_on_replaces_active_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 90);
  const VoiceAction action = instrument.noteOn(67, 80);

  assertVoiceAction(action, VoiceActionType::StartNote, 67, 80);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(67, instrument.midiNoteNumber());
}

void test_releasing_current_note_returns_to_previous_held_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 72);
  instrument.noteOn(64, 110);
  const VoiceAction action = instrument.noteOff(64);

  assertVoiceAction(action, VoiceActionType::StartNote, 60, 72);
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

  assertVoiceAction(action, VoiceActionType::StartNote, 64, 100);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, instrument.midiNoteNumber());
}

void test_midi_note_zero_is_valid() {
  MonophonicInstrument instrument;
  char noteName[5];

  const VoiceAction startAction = instrument.noteOn(0, 100);
  instrument.noteName(noteName, sizeof(noteName));

  assertVoiceAction(startAction, VoiceActionType::StartNote, 0, 100);
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

void test_typed_note_on_event_starts_note() {
  MonophonicInstrument instrument;
  const NoteEvent event = {NoteEventType::NoteOn, 3, 65, 88};

  const VoiceAction action = instrument.handleNoteEvent(event);

  assertVoiceAction(action, VoiceActionType::StartNote, 65, 88);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(65, instrument.midiNoteNumber());
}

void test_typed_note_off_event_stops_matching_note() {
  MonophonicInstrument instrument;

  instrument.handleNoteEvent({NoteEventType::NoteOn, 2, 65, 88});
  const VoiceAction action =
      instrument.handleNoteEvent({NoteEventType::NoteOff, 2, 65, 0});

  assertVoiceAction(action, VoiceActionType::StopNote, 65);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
}

void test_typed_note_on_event_with_zero_velocity_is_note_off() {
  MonophonicInstrument instrument;

  instrument.handleNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  const VoiceAction action =
      instrument.handleNoteEvent({NoteEventType::NoteOn, 1, 60, 0});

  assertVoiceAction(action, VoiceActionType::StopNote, 60);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
}

void test_typed_note_event_channel_is_currently_ignored() {
  MonophonicInstrument instrument;

  instrument.handleNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  const VoiceAction action =
      instrument.handleNoteEvent({NoteEventType::NoteOff, 16, 60, 0});

  assertVoiceAction(action, VoiceActionType::StopNote, 60);
  TEST_ASSERT_FALSE(instrument.isNoteActive());
}

void test_latest_velocity_is_used_for_repeated_note_on() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 40);
  instrument.noteOn(64, 100);
  instrument.noteOn(60, 90);
  const VoiceAction action = instrument.noteOff(60);

  assertVoiceAction(action, VoiceActionType::StartNote, 64, 100);
  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, instrument.midiNoteNumber());
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

void test_over_capacity_discards_oldest_held_note() {
  MonophonicInstrument instrument;

  for (uint8_t note = 0; note <= 16; ++note) {
    instrument.noteOn(note, 100);
  }

  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(16, instrument.midiNoteNumber());

  for (uint8_t note = 16; note >= 1; --note) {
    const VoiceAction action = instrument.noteOff(note);
    const uint8_t expectedNextNote = note - 1;

    if (note > 1) {
      assertVoiceAction(action, VoiceActionType::StartNote, expectedNextNote, 100);
      TEST_ASSERT_TRUE(instrument.isNoteActive());
      TEST_ASSERT_EQUAL_UINT8(expectedNextNote, instrument.midiNoteNumber());
    } else {
      assertVoiceAction(action, VoiceActionType::StopNote, 1);
      TEST_ASSERT_FALSE(instrument.isNoteActive());
    }
  }

  const VoiceAction discardedNoteOffAction = instrument.noteOff(0);
  assertVoiceAction(discardedNoteOffAction, VoiceActionType::None, 0);
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

void test_pitch_bend_zero_keeps_base_frequency() {
  MonophonicInstrument instrument;

  const VoiceAction noteAction = instrument.noteOn(60, 100);
  const VoiceAction bendAction = instrument.handlePitchBendEvent({1, 0});

  assertStartFrequency(noteAction, midiNoteToFrequencyHz(60));
  assertStartFrequency(bendAction, midiNoteToFrequencyHz(60));
  TEST_ASSERT_EQUAL_INT16(0, instrument.pitchBendValue());
  TEST_ASSERT_FLOAT_WITHIN(0.01f, midiNoteToFrequencyHz(60), instrument.frequencyHz());
}

void test_pitch_bend_maximum_raises_two_semitones() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.handlePitchBendEvent({1, 8191});

  TEST_ASSERT_EQUAL_INT16(8191, instrument.pitchBendValue());
  assertStartFrequency(action, midiNoteToFrequencyHz(62));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, midiNoteToFrequencyHz(62), instrument.frequencyHz());
}

void test_pitch_bend_minimum_lowers_two_semitones() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.handlePitchBendEvent({1, -8192});

  TEST_ASSERT_EQUAL_INT16(-8192, instrument.pitchBendValue());
  assertStartFrequency(action, midiNoteToFrequencyHz(58));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, midiNoteToFrequencyHz(58), instrument.frequencyHz());
}

void test_pitch_bend_range_can_be_configured() {
  MonophonicInstrument instrument;

  instrument.setPitchBendRangeSemitones(12.0f);
  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.handlePitchBendEvent({1, 8191});

  TEST_ASSERT_FLOAT_WITHIN(0.01f, 12.0f, instrument.pitchBendRangeSemitones());
  assertStartFrequency(action, midiNoteToFrequencyHz(72));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, midiNoteToFrequencyHz(72), instrument.frequencyHz());
}

void test_negative_pitch_bend_range_is_clamped_to_zero() {
  MonophonicInstrument instrument;

  instrument.setPitchBendRangeSemitones(-1.0f);
  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.handlePitchBendEvent({1, 8191});

  TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, instrument.pitchBendRangeSemitones());
  assertStartFrequency(action, midiNoteToFrequencyHz(60));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, midiNoteToFrequencyHz(60), instrument.frequencyHz());
}

void test_pitch_bend_dead_zone_treats_near_center_as_zero() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  const VoiceAction action = instrument.handlePitchBendEvent({1, 64});

  TEST_ASSERT_EQUAL_INT16(0, instrument.pitchBendValue());
  assertStartFrequency(action, midiNoteToFrequencyHz(60));
}

void test_pitch_bend_without_active_note_is_applied_to_next_note() {
  MonophonicInstrument instrument;

  const VoiceAction bendAction = instrument.handlePitchBendEvent({1, 8191});
  const VoiceAction noteAction = instrument.noteOn(60, 100);

  assertVoiceAction(bendAction, VoiceActionType::None, 0);
  assertStartFrequency(noteAction, midiNoteToFrequencyHz(62));
}

void test_stop_all_resets_pitch_bend() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.handlePitchBendEvent({1, 8191});
  instrument.stopAll();
  const VoiceAction action = instrument.noteOn(60, 100);

  TEST_ASSERT_EQUAL_INT16(0, instrument.pitchBendValue());
  assertStartFrequency(action, midiNoteToFrequencyHz(60));
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
  RUN_TEST(test_typed_note_on_event_starts_note);
  RUN_TEST(test_typed_note_off_event_stops_matching_note);
  RUN_TEST(test_typed_note_on_event_with_zero_velocity_is_note_off);
  RUN_TEST(test_typed_note_event_channel_is_currently_ignored);
  RUN_TEST(test_latest_velocity_is_used_for_repeated_note_on);
  RUN_TEST(test_stop_all_silences_active_note_and_clears_held_notes);
  RUN_TEST(test_over_capacity_discards_oldest_held_note);
  RUN_TEST(test_frequency_and_name_follow_active_note);
  RUN_TEST(test_pitch_bend_zero_keeps_base_frequency);
  RUN_TEST(test_pitch_bend_maximum_raises_two_semitones);
  RUN_TEST(test_pitch_bend_minimum_lowers_two_semitones);
  RUN_TEST(test_pitch_bend_range_can_be_configured);
  RUN_TEST(test_negative_pitch_bend_range_is_clamped_to_zero);
  RUN_TEST(test_pitch_bend_dead_zone_treats_near_center_as_zero);
  RUN_TEST(test_pitch_bend_without_active_note_is_applied_to_next_note);
  RUN_TEST(test_stop_all_resets_pitch_bend);
  RUN_TEST(test_waveform_toggle_alternates_between_saw_and_square);
  return UNITY_END();
}
