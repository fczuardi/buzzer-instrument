#include <Arduino.h>
#include <M5Unified.h>

#include "MonophonicInstrument.h"
#include "SpeakerToneOutput.h"

namespace {
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;

MonophonicInstrument instrument;
SpeakerToneOutput speakerToneOutput;
bool tonePlaying = false;
uint32_t lastUptimeLogAtMs = 0;

void drawStaticScreen() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("Buzzer");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println("MIDI-like note event test");
  M5.Display.println();
  M5.Display.println("BtnA: C4 note on/off");
  M5.Display.println("BtnB: waveform");
  M5.Display.println("MIDI-like event test");
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
  instrument.noteName(noteName, sizeof(noteName));

  M5.Display.println("Output: M5.Speaker");
  M5.Display.print("Wave: ");
  M5.Display.println(instrument.waveformName());
  M5.Display.printf(
      "Note: %s (%u)\n",
      noteName,
      instrument.midiNoteNumber());
  M5.Display.print("Freq: ");
  M5.Display.print(instrument.frequencyHz(), 2);
  M5.Display.println(" Hz");
  M5.Display.print("State: ");
  M5.Display.println(stateLabel);
}

void startActiveNote() {
  if (!instrument.isNoteActive()) {
    return;
  }

  char noteName[5];
  instrument.noteName(noteName, sizeof(noteName));

  speakerToneOutput.setWaveform(instrument.waveform());
  const float frequencyHz = instrument.frequencyHz();
  const bool toneStarted = speakerToneOutput.startTone(frequencyHz);
  tonePlaying = toneStarted;

  Serial.printf(
      "buzzer: tone_start ok=%s backend=m5speaker waveform=%s note=%s midi_note=%u frequency_hz=%.2f\n",
      toneStarted ? "true" : "false",
      speakerToneOutput.waveformName(),
      noteName,
      instrument.midiNoteNumber(),
      frequencyHz);
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
  speakerToneOutput.setWaveform(instrument.waveform());

  Serial.println();
  Serial.println("M5StickC Plus2 buzzer instrument");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.printf(
      "buzzer: backend=m5speaker pin=2 volume=128 magnification=32 sample_rate_hz=48000 waveform=%s\n",
      speakerToneOutput.waveformName());

  drawStaticScreen();
  drawToneState("idle");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    instrument.noteOn(MonophonicInstrument::DEFAULT_TEST_NOTE, 100);
    startActiveNote();
  }

  if (M5.BtnA.wasReleased()) {
    instrument.noteOff(MonophonicInstrument::DEFAULT_TEST_NOTE);
    stopTone("note_off");
  }

  if (M5.BtnB.wasClicked()) {
    const bool wasPlaying = tonePlaying;
    if (wasPlaying) {
      stopTone("next_waveform");
    }

    instrument.selectNextWaveform();
    char noteName[5];
    instrument.noteName(noteName, sizeof(noteName));
    Serial.printf(
        "buzzer: waveform_selected waveform=%s note=%s midi_note=%u frequency_hz=%.2f\n",
        instrument.waveformName(),
        noteName,
        instrument.midiNoteNumber(),
        instrument.frequencyHz());

    if (wasPlaying) {
      startActiveNote();
    } else {
      drawToneState("idle");
    }
  }

  const uint32_t nowMs = millis();
  if (nowMs - lastUptimeLogAtMs >= UPTIME_LOG_INTERVAL_MS) {
    lastUptimeLogAtMs = nowMs;
    Serial.printf("uptime=%lu\n", nowMs / 1000);
  }
}
