#include <Arduino.h>
#include <M5Unified.h>

#include "MidiNote.h"
#include "SpeakerToneOutput.h"

namespace {
constexpr uint32_t SMOKE_TEST_DURATION_MS = 300;
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;
constexpr uint8_t FIRST_DEMO_MIDI_NOTE = 60;  // C4 with the 60=C4 convention.
constexpr uint8_t LAST_DEMO_MIDI_NOTE = 108;  // C8 keeps the range piano-like.

SpeakerToneOutput speakerToneOutput;
bool tonePlaying = false;
uint8_t selectedMidiNoteNumber = FIRST_DEMO_MIDI_NOTE;
uint32_t toneStartedAtMs = 0;
uint32_t lastUptimeLogAtMs = 0;

void selectedNoteName(char* output, size_t outputSize) {
  midiNoteName(selectedMidiNoteNumber, output, outputSize);
}

float selectedFrequencyHz() {
  return midiNoteToFrequencyHz(selectedMidiNoteNumber);
}

void selectNote(uint8_t midiNoteNumber) {
  selectedMidiNoteNumber = midiNoteNumber;
}

void resetToFirstNote() {
  selectNote(FIRST_DEMO_MIDI_NOTE);
}

void advanceSemitone() {
  if (selectedMidiNoteNumber >= LAST_DEMO_MIDI_NOTE) {
    resetToFirstNote();
    return;
  }

  selectNote(selectedMidiNoteNumber + 1);
}

void selectNextWaveform() {
  using Waveform = SpeakerToneOutput::Waveform;

  switch (speakerToneOutput.waveform()) {
    case Waveform::Square:
      speakerToneOutput.setWaveform(Waveform::Pulse);
      break;
    case Waveform::Pulse:
      speakerToneOutput.setWaveform(Waveform::Saw);
      break;
    case Waveform::Saw:
      speakerToneOutput.setWaveform(Waveform::SawDown);
      break;
    case Waveform::SawDown:
      speakerToneOutput.setWaveform(Waveform::Square);
      break;
  }
}

void drawStaticScreen() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("Buzzer");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println("MIDI note frequency test");
  M5.Display.println();
  M5.Display.println("BtnA: +1 semitone");
  M5.Display.println("BtnB: waveform");
  M5.Display.println("Hold A: C4");
}

void drawToneState(const char* stateLabel) {
  constexpr int32_t STATE_ROW_Y = 72;
  constexpr int32_t STATE_ROW_HEIGHT = 48;

  M5.Display.fillRect(
      0,
      STATE_ROW_Y,
      M5.Display.width(),
      STATE_ROW_HEIGHT,
      TFT_BLACK);
  M5.Display.setCursor(0, STATE_ROW_Y);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  char noteName[5];
  selectedNoteName(noteName, sizeof(noteName));

  M5.Display.println("Output: M5.Speaker");
  M5.Display.print("Wave: ");
  M5.Display.println(speakerToneOutput.waveformName());
  M5.Display.printf(
      "Note: %s (%u)\n",
      noteName,
      selectedMidiNoteNumber);
  M5.Display.print("Freq: ");
  M5.Display.print(selectedFrequencyHz(), 2);
  M5.Display.println(" Hz");
  M5.Display.print("State: ");
  M5.Display.print(stateLabel);
}

void startSelectedNote() {
  char noteName[5];
  selectedNoteName(noteName, sizeof(noteName));

  const float frequencyHz = selectedFrequencyHz();
  const bool toneStarted = speakerToneOutput.startTone(frequencyHz);
  tonePlaying = toneStarted;
  toneStartedAtMs = millis();

  Serial.printf(
      "buzzer: tone_start ok=%s backend=m5speaker waveform=%s note=%s midi_note=%u frequency_hz=%.2f duration_ms=%lu\n",
      toneStarted ? "true" : "false",
      speakerToneOutput.waveformName(),
      noteName,
      selectedMidiNoteNumber,
      frequencyHz,
      SMOKE_TEST_DURATION_MS);
  drawToneState(toneStarted ? "playing" : "failed");
}

void stopTone(const char* reason) {
  if (!tonePlaying) {
    return;
  }

  speakerToneOutput.stop();
  tonePlaying = false;

  Serial.print("buzzer: tone_stop reason=");
  Serial.println(reason);
  drawToneState("idle");
}
}

void setup() {
  auto config = M5.config();
  config.internal_spk = true;
  config.internal_mic = false;
  config.fallback_board = m5::board_t::board_M5StickCPlus2;
  M5.begin(config);

  Serial.begin(115200);
  delay(200);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(96);

  speakerToneOutput.begin();

  Serial.println();
  Serial.println("M5StickC Plus2 buzzer instrument");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.printf(
      "buzzer: backend=m5speaker pin=2 volume=128 magnification=32 sample_rate_hz=48000 waveform=%s\n",
      speakerToneOutput.waveformName());

  drawStaticScreen();
  drawToneState("idle");
  startSelectedNote();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasHold()) {
    stopTone("reset_to_c4");
    resetToFirstNote();
    startSelectedNote();
  } else if (M5.BtnA.wasClicked()) {
    stopTone("advance_semitone");
    advanceSemitone();
    startSelectedNote();
  }

  if (M5.BtnB.wasClicked()) {
    stopTone("next_waveform");
    selectNextWaveform();
    char noteName[5];
    selectedNoteName(noteName, sizeof(noteName));
    Serial.printf(
        "buzzer: waveform_selected waveform=%s note=%s midi_note=%u frequency_hz=%.2f\n",
        speakerToneOutput.waveformName(),
        noteName,
        selectedMidiNoteNumber,
        selectedFrequencyHz());
    startSelectedNote();
  }

  if (tonePlaying && millis() - toneStartedAtMs >= SMOKE_TEST_DURATION_MS) {
    stopTone("duration_elapsed");
  }

  const uint32_t nowMs = millis();
  if (nowMs - lastUptimeLogAtMs >= UPTIME_LOG_INTERVAL_MS) {
    lastUptimeLogAtMs = nowMs;
    Serial.printf("uptime=%lu\n", nowMs / 1000);
  }
}
