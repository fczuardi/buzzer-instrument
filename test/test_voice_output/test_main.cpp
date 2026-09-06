#include <unity.h>

#include "VoiceOutput.h"

class CapturingVoiceOutput : public VoiceOutput {
public:
  bool startNote(
      uint8_t midiNote,
      ToneWaveform waveform,
      uint8_t velocity) override {
    lastMidiNote = midiNote;
    lastWaveform = waveform;
    lastVelocity = velocity;
    playing = true;
    return true;
  }

  void stopNote() override {
    playing = false;
  }

  bool isPlaying() const override {
    return playing;
  }

  uint8_t lastMidiNote = 0;
  uint8_t lastVelocity = 0;
  ToneWaveform lastWaveform = ToneWaveform::Square32;
  bool playing = false;
};

void test_voice_output_starts_a_note_with_waveform() {
  CapturingVoiceOutput output;

  const bool started = output.startNote(60, ToneWaveform::Saw32, 96);

  TEST_ASSERT_TRUE(started);
  TEST_ASSERT_TRUE(output.isPlaying());
  TEST_ASSERT_EQUAL_UINT8(60, output.lastMidiNote);
  TEST_ASSERT_EQUAL_UINT8(96, output.lastVelocity);
  TEST_ASSERT_EQUAL(ToneWaveform::Saw32, output.lastWaveform);
}

void test_voice_output_stops_current_note() {
  CapturingVoiceOutput output;

  output.startNote(64, ToneWaveform::Square32, 100);
  output.stopNote();

  TEST_ASSERT_FALSE(output.isPlaying());
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_voice_output_starts_a_note_with_waveform);
  RUN_TEST(test_voice_output_stops_current_note);
  return UNITY_END();
}
