#include <unity.h>

#include "MonophonicInstrument.h"

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

  instrument.noteOn(64, 96);
  instrument.noteName(noteName, sizeof(noteName));

  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(64, instrument.midiNoteNumber());
  TEST_ASSERT_EQUAL_STRING("E4", noteName);
}

void test_latest_note_on_replaces_active_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 90);
  instrument.noteOn(67, 80);

  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(67, instrument.midiNoteNumber());
}

void test_matching_note_off_stops_active_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.noteOff(60);

  TEST_ASSERT_FALSE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(60, instrument.midiNoteNumber());
}

void test_note_off_for_other_note_is_ignored() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.noteOff(62);

  TEST_ASSERT_TRUE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(60, instrument.midiNoteNumber());
}

void test_note_on_with_zero_velocity_is_note_off() {
  MonophonicInstrument instrument;

  instrument.noteOn(60, 100);
  instrument.noteOn(60, 0);

  TEST_ASSERT_FALSE(instrument.isNoteActive());
}

void test_stop_all_silences_active_note() {
  MonophonicInstrument instrument;

  instrument.noteOn(72, 100);
  instrument.stopAll();

  TEST_ASSERT_FALSE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(72, instrument.midiNoteNumber());
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
  RUN_TEST(test_matching_note_off_stops_active_note);
  RUN_TEST(test_note_off_for_other_note_is_ignored);
  RUN_TEST(test_note_on_with_zero_velocity_is_note_off);
  RUN_TEST(test_stop_all_silences_active_note);
  RUN_TEST(test_frequency_and_name_follow_active_note);
  RUN_TEST(test_waveform_toggle_alternates_between_saw_and_square);
  return UNITY_END();
}
