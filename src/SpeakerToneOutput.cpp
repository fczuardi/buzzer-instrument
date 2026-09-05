#include "SpeakerToneOutput.h"

#include <Arduino.h>
#include <M5Unified.h>

namespace {
// One-cycle 8-bit square wave used by M5Unified's sampled-tone path.
//
// Hardware testing showed that the M5StickC Plus2 buzzer needs conservative
// gain, but works well with this simple waveform once volume is lowered.
constexpr uint8_t SQUARE_WAVE_16[] = {
    255, 255, 255, 255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,   0,   0,   0,
};
}

void SpeakerToneOutput::begin() {
  auto config = M5.Speaker.config();
  config.buzzer = true;
  config.pin_data_out = BUZZER_GPIO_PIN;
  config.sample_rate = SPEAKER_SAMPLE_RATE_HZ;
  config.magnification = SPEAKER_MAGNIFICATION;
  M5.Speaker.config(config);

  M5.Speaker.begin();
  M5.Speaker.setVolume(SPEAKER_VOLUME);
  initialized_ = true;
  playing_ = false;
}

void SpeakerToneOutput::end() {
  if (!initialized_) {
    return;
  }

  stop();
  M5.Speaker.end();
  initialized_ = false;
}

bool SpeakerToneOutput::startTone(uint32_t frequencyHz) {
  if (!initialized_ || frequencyHz == 0) {
    return false;
  }

  const bool toneStarted = M5.Speaker.tone(
      static_cast<float>(frequencyHz),
      UINT32_MAX,
      SPEAKER_CHANNEL,
      true,
      SQUARE_WAVE_16,
      sizeof(SQUARE_WAVE_16));

  playing_ = toneStarted;
  return toneStarted;
}

void SpeakerToneOutput::stop() {
  if (!initialized_) {
    return;
  }

  M5.Speaker.stop(SPEAKER_CHANNEL);
  playing_ = false;
}

bool SpeakerToneOutput::isPlaying() const {
  return playing_;
}
