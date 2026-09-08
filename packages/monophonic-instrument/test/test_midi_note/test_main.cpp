#include <unity.h>

#include "MidiNote.h"

void test_a4_is_440_hz() {
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 440.0f, midiNoteToFrequencyHz(69));
}

void test_c4_matches_equal_temperament_frequency() {
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 261.63f, midiNoteToFrequencyHz(60));
}

void test_adjacent_octaves_double_or_halve_frequency() {
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 220.0f, midiNoteToFrequencyHz(57));
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 880.0f, midiNoteToFrequencyHz(81));
}

void test_note_names_use_c4_as_midi_60() {
  char noteName[5];

  midiNoteName(60, noteName, sizeof(noteName));
  TEST_ASSERT_EQUAL_STRING("C4", noteName);

  midiNoteName(61, noteName, sizeof(noteName));
  TEST_ASSERT_EQUAL_STRING("C#4", noteName);

  midiNoteName(108, noteName, sizeof(noteName));
  TEST_ASSERT_EQUAL_STRING("C8", noteName);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_a4_is_440_hz);
  RUN_TEST(test_c4_matches_equal_temperament_frequency);
  RUN_TEST(test_adjacent_octaves_double_or_halve_frequency);
  RUN_TEST(test_note_names_use_c4_as_midi_60);
  return UNITY_END();
}
