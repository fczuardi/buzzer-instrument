#include "BuzzerOutput.h"

#include <Arduino.h>

void BuzzerOutput::begin() {
  ledcSetup(PWM_CHANNEL, 1, PWM_RESOLUTION_BITS);
  ledcAttachPin(BUZZER_GPIO_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0);
  initialized_ = true;
  playing_ = false;
}

bool BuzzerOutput::startTone(uint32_t frequencyHz) {
  if (!initialized_ || frequencyHz == 0) {
    return false;
  }

  const uint32_t actualFrequencyHz = ledcWriteTone(PWM_CHANNEL, frequencyHz);
  if (actualFrequencyHz == 0) {
    playing_ = false;
    return false;
  }

  // ledcWriteTone updates the timer frequency. A non-zero duty cycle is still
  // needed to drive the passive buzzer pin.
  ledcWrite(PWM_CHANNEL, PWM_DUTY_50_PERCENT);
  playing_ = true;
  return true;
}

void BuzzerOutput::stop() {
  if (!initialized_) {
    return;
  }

  ledcWrite(PWM_CHANNEL, 0);
  ledcWriteTone(PWM_CHANNEL, 0);
  playing_ = false;
}

bool BuzzerOutput::isPlaying() const {
  return playing_;
}
