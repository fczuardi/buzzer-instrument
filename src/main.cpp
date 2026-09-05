#include <Arduino.h>
#include <M5Unified.h>

#include "BuzzerOutput.h"

namespace {
constexpr uint32_t SMOKE_TEST_FREQUENCY_HZ = 440;
constexpr uint32_t SMOKE_TEST_DURATION_MS = 500;
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;

BuzzerOutput buzzerOutput;
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
  M5.Display.println("A4 440 Hz smoke test");
  M5.Display.println();
  M5.Display.println("BtnA: play");
  M5.Display.println("BtnB: stop");
}

void drawToneState(const char* stateLabel) {
  constexpr int32_t STATE_ROW_Y = 72;
  constexpr int32_t STATE_ROW_HEIGHT = 16;

  M5.Display.fillRect(
      0,
      STATE_ROW_Y,
      M5.Display.width(),
      STATE_ROW_HEIGHT,
      TFT_BLACK);
  M5.Display.setCursor(0, STATE_ROW_Y);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.print("State: ");
  M5.Display.print(stateLabel);
}

void startSmokeTestTone() {
  const bool toneStarted = buzzerOutput.startTone(SMOKE_TEST_FREQUENCY_HZ);
  tonePlaying = true;
  toneStartedAtMs = millis();

  Serial.printf(
      "buzzer: tone_start ok=%s backend=ledc frequency_hz=%lu duration_ms=%lu\n",
      toneStarted ? "true" : "false",
      SMOKE_TEST_FREQUENCY_HZ,
      SMOKE_TEST_DURATION_MS);
  drawToneState("playing");
}

void stopTone(const char* reason) {
  if (!tonePlaying) {
    return;
  }

  buzzerOutput.stop();
  tonePlaying = false;

  Serial.print("buzzer: tone_stop reason=");
  Serial.println(reason);
  drawToneState("idle");
}
}

void setup() {
  auto config = M5.config();
  config.internal_spk = false;
  config.internal_mic = false;
  config.fallback_board = m5::board_t::board_M5StickCPlus2;
  M5.begin(config);

  Serial.begin(115200);
  delay(200);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(96);

  buzzerOutput.begin();

  Serial.println();
  Serial.println("M5StickC Plus2 buzzer instrument");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.println("buzzer: backend=ledc pin=2 channel=0 duty=50%");

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
