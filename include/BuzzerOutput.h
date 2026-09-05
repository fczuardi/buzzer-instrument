#pragma once

#include <cstdint>

// Drives a passive buzzer directly with ESP32 LEDC PWM.
//
// This backend intentionally knows nothing about notes or MIDI. It only starts
// and stops a frequency on one GPIO pin.
class BuzzerOutput {
public:
  void begin();
  bool startTone(uint32_t frequencyHz);
  void stop();
  bool isPlaying() const;

private:
  static constexpr uint8_t PWM_CHANNEL = 0;
  static constexpr uint8_t PWM_RESOLUTION_BITS = 10;
  static constexpr uint32_t PWM_DUTY_50_PERCENT = 1 << (PWM_RESOLUTION_BITS - 1);
  static constexpr uint8_t BUZZER_GPIO_PIN = 2;

  bool initialized_ = false;
  bool playing_ = false;
};
