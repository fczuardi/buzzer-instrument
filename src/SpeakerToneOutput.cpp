#include "SpeakerToneOutput.h"

#include <Arduino.h>
#include <M5Unified.h>

namespace {
struct WaveformDefinition {
  SpeakerToneOutput::Waveform waveform;
  const char* name;
  const uint8_t* samples;
  size_t sampleCount;
};

// One-cycle 8-bit wave tables used by M5Unified's sampled-tone path.
// Hardware testing showed that the M5StickC Plus2 buzzer needs conservative
// gain, and that waveform shape strongly affects perceived musical usefulness.
constexpr uint8_t SQUARE_WAVE_16[] = {
    255, 255, 255, 255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr uint8_t PULSE_WAVE_16[] = {
    255, 255, 0, 0, 0, 0, 0, 0,
    0,   0,   0, 0, 0, 0, 0, 0,
};

constexpr uint8_t SAW_WAVE_16[] = {
    0,   17,  34,  51,  68,  85,  102, 119,
    136, 153, 170, 187, 204, 221, 238, 255,
};

constexpr uint8_t SAW_DOWN_WAVE_16[] = {
    255, 238, 221, 204, 187, 170, 153, 136,
    119, 102, 85,  68,  51,  34,  17,  0,
};

constexpr WaveformDefinition WAVEFORMS[] = {
    {SpeakerToneOutput::Waveform::Square, "square16", SQUARE_WAVE_16, sizeof(SQUARE_WAVE_16)},
    {SpeakerToneOutput::Waveform::Pulse, "pulse16", PULSE_WAVE_16, sizeof(PULSE_WAVE_16)},
    {SpeakerToneOutput::Waveform::Saw, "saw16", SAW_WAVE_16, sizeof(SAW_WAVE_16)},
    {SpeakerToneOutput::Waveform::SawDown, "sawdown16", SAW_DOWN_WAVE_16, sizeof(SAW_DOWN_WAVE_16)},
};

const WaveformDefinition& waveformDefinition(SpeakerToneOutput::Waveform waveform) {
  for (const WaveformDefinition& definition : WAVEFORMS) {
    if (definition.waveform == waveform) {
      return definition;
    }
  }

  return WAVEFORMS[0];
}
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

bool SpeakerToneOutput::startTone(float frequencyHz) {
  if (!initialized_ || frequencyHz <= 0.0f) {
    return false;
  }

  const WaveformDefinition& definition = waveformDefinition(waveform_);
  const bool toneStarted = M5.Speaker.tone(
      frequencyHz,
      UINT32_MAX,
      SPEAKER_CHANNEL,
      true,
      definition.samples,
      definition.sampleCount);

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

void SpeakerToneOutput::setWaveform(Waveform waveform) {
  waveform_ = waveform;
}

SpeakerToneOutput::Waveform SpeakerToneOutput::waveform() const {
  return waveform_;
}

const char* SpeakerToneOutput::waveformName() const {
  return waveformDefinition(waveform_).name;
}
