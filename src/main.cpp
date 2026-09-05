#include <Arduino.h>
#include <M5Unified.h>

#include "SpeakerToneOutput.h"

namespace {
constexpr uint32_t SMOKE_TEST_FREQUENCY_HZ = 440;
constexpr uint32_t SMOKE_TEST_DURATION_MS = 500;
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;

SpeakerToneOutput speakerToneOutput;
bool tonePlaying = false;
uint32_t toneStartedAtMs = 0;
uint32_t lastUptimeLogAtMs = 0;

void drawStaticScreen() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("Buzzer");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println("M5Speaker A4 test");
  M5.Display.println();
  M5.Display.println("BtnA: play");
  M5.Display.println("BtnB: stop");
}

void drawToneState(const char* stateLabel) {
  constexpr int32_t STATE_ROW_Y = 72;
  constexpr int32_t STATE_ROW_HEIGHT = 32;

  M5.Display.fillRect(
      0,
      STATE_ROW_Y,
      M5.Display.width(),
      STATE_ROW_HEIGHT,
      TFT_BLACK);
  M5.Display.setCursor(0, STATE_ROW_Y);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println("Output: M5.Speaker");
  M5.Display.print("State: ");
  M5.Display.print(stateLabel);
}

void startSmokeTestTone() {
  const bool toneStarted = speakerToneOutput.startTone(SMOKE_TEST_FREQUENCY_HZ);
  tonePlaying = toneStarted;
  toneStartedAtMs = millis();

  Serial.printf(
      "buzzer: tone_start ok=%s backend=m5speaker frequency_hz=%lu duration_ms=%lu\n",
      toneStarted ? "true" : "false",
      SMOKE_TEST_FREQUENCY_HZ,
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
  Serial.println("buzzer: backend=m5speaker pin=2 volume=128 magnification=32 sample_rate_hz=48000 waveform=square16");

  drawStaticScreen();
  drawToneState("idle");
  startSmokeTestTone();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    startSmokeTestTone();
  }

  if (M5.BtnB.wasPressed()) {
    stopTone("button_b");
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
