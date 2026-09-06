#include <unity.h>

#include "MonophonicInstrumentSink.h"

class CapturingVoiceOutput : public VoiceOutput {
public:
  bool startNote(uint8_t midiNote, ToneWaveform waveform) override {
    lastMidiNote = midiNote;
    lastWaveform = waveform;
    playing = true;
    ++startCount;
    return true;
  }

  void stopNote() override {
    playing = false;
    ++stopCount;
  }

  bool isPlaying() const override {
    return playing;
  }

  uint8_t lastMidiNote = 0;
  ToneWaveform lastWaveform = ToneWaveform::Square32;
  bool playing = false;
  uint8_t startCount = 0;
  uint8_t stopCount = 0;
};

void test_note_on_starts_voice_with_instrument_waveform() {
  MonophonicInstrument instrument;
  CapturingVoiceOutput output;
  MonophonicInstrumentSink sink(instrument, output);

  sink.onNoteEvent({NoteEventType::NoteOn, 1, 60, 100});

  TEST_ASSERT_TRUE(output.isPlaying());
  TEST_ASSERT_EQUAL_UINT8(1, output.startCount);
  TEST_ASSERT_EQUAL_UINT8(60, output.lastMidiNote);
  TEST_ASSERT_EQUAL(ToneWaveform::Saw32, output.lastWaveform);
}

void test_note_off_stops_matching_active_voice() {
  MonophonicInstrument instrument;
  CapturingVoiceOutput output;
  MonophonicInstrumentSink sink(instrument, output);

  sink.onNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  sink.onNoteEvent({NoteEventType::NoteOff, 1, 60, 0});

  TEST_ASSERT_FALSE(output.isPlaying());
  TEST_ASSERT_EQUAL_UINT8(1, output.stopCount);
}

void test_releasing_current_note_restarts_previous_held_note() {
  MonophonicInstrument instrument;
  CapturingVoiceOutput output;
  MonophonicInstrumentSink sink(instrument, output);

  sink.onNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  sink.onNoteEvent({NoteEventType::NoteOn, 1, 64, 100});
  sink.onNoteEvent({NoteEventType::NoteOff, 1, 64, 0});

  TEST_ASSERT_TRUE(output.isPlaying());
  TEST_ASSERT_EQUAL_UINT8(3, output.startCount);
  TEST_ASSERT_EQUAL_UINT8(60, output.lastMidiNote);
}

void test_note_off_for_non_current_note_does_not_touch_output() {
  MonophonicInstrument instrument;
  CapturingVoiceOutput output;
  MonophonicInstrumentSink sink(instrument, output);

  sink.onNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  sink.onNoteEvent({NoteEventType::NoteOn, 1, 64, 100});
  sink.onNoteEvent({NoteEventType::NoteOff, 1, 60, 0});

  TEST_ASSERT_TRUE(output.isPlaying());
  TEST_ASSERT_EQUAL_UINT8(2, output.startCount);
  TEST_ASSERT_EQUAL_UINT8(0, output.stopCount);
  TEST_ASSERT_EQUAL_UINT8(64, output.lastMidiNote);
}

void test_disconnected_stops_output_and_clears_held_notes() {
  MonophonicInstrument instrument;
  CapturingVoiceOutput output;
  MonophonicInstrumentSink sink(instrument, output);

  sink.onNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  sink.onNoteEvent({NoteEventType::NoteOn, 1, 64, 100});
  sink.onDisconnected();
  sink.onNoteEvent({NoteEventType::NoteOff, 1, 60, 0});

  TEST_ASSERT_FALSE(output.isPlaying());
  TEST_ASSERT_FALSE(instrument.isNoteActive());
  TEST_ASSERT_EQUAL_UINT8(1, output.stopCount);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_note_on_starts_voice_with_instrument_waveform);
  RUN_TEST(test_note_off_stops_matching_active_voice);
  RUN_TEST(test_releasing_current_note_restarts_previous_held_note);
  RUN_TEST(test_note_off_for_non_current_note_does_not_touch_output);
  RUN_TEST(test_disconnected_stops_output_and_clears_held_notes);
  return UNITY_END();
}
