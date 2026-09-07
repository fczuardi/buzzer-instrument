#include "SpeakerToneOutput.h"

#include <cstddef>

#include <Arduino.h>
#include <M5Unified.h>

namespace {
struct WaveformDefinition {
  ToneWaveform waveform;
  const uint8_t* samples;
  size_t sampleCount;
};

constexpr uint8_t SQUARE_WAVE_32[] = {
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr uint8_t SAW_WAVE_32[] = {
    0,   8,   16,  25,  33,  41,  49,  58,
    66,  74,  82,  90,  99,  107, 115, 123,
    132, 140, 148, 156, 165, 173, 181, 189,
    197, 206, 214, 222, 230, 239, 247, 255,
};

constexpr WaveformDefinition WAVEFORMS[] = {
    {ToneWaveform::Square32, SQUARE_WAVE_32, sizeof(SQUARE_WAVE_32)},
    {ToneWaveform::Saw32, SAW_WAVE_32, sizeof(SAW_WAVE_32)},
};

const WaveformDefinition& waveformDefinition(ToneWaveform waveform) {
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
  M5.Speaker.setVolume(volume_);
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

bool SpeakerToneOutput::startNote(
    uint8_t,
    float frequencyHz,
    ToneWaveform waveform,
    uint8_t velocity) {
  setWaveform(waveform);
  setVolume(volumeForVelocity(velocity));
  return startTone(frequencyHz);
}

void SpeakerToneOutput::stop() {
  if (!initialized_) {
    return;
  }

  M5.Speaker.stop(SPEAKER_CHANNEL);
  playing_ = false;
}

void SpeakerToneOutput::setVolume(uint8_t volume) {
  volume_ = volume;
  if (initialized_) {
    M5.Speaker.setVolume(volume_);
  }
}

uint8_t SpeakerToneOutput::volume() const {
  return volume_;
}

void SpeakerToneOutput::setVelocityVolumeRange(VelocityVolumeRange range) {
  if (range.minimum > range.maximum) {
    const uint8_t originalMinimum = range.minimum;
    range.minimum = range.maximum;
    range.maximum = originalMinimum;
  }

  velocityVolumeRange_ = range;
}

VelocityVolumeRange SpeakerToneOutput::velocityVolumeRange() const {
  return velocityVolumeRange_;
}

uint8_t SpeakerToneOutput::volumeForVelocity(uint8_t velocity) const {
  return velocityVolumeRange_.minimum +
      ((static_cast<uint16_t>(velocity) *
        (velocityVolumeRange_.maximum - velocityVolumeRange_.minimum)) /
       127);
}

void SpeakerToneOutput::stopNote() {
  stop();
}

bool SpeakerToneOutput::isPlaying() const {
  return playing_;
}

void SpeakerToneOutput::setWaveform(ToneWaveform waveform) {
  waveform_ = waveform;
}

ToneWaveform SpeakerToneOutput::waveform() const {
  return waveform_;
}

const char* SpeakerToneOutput::waveformName() const {
  return toneWaveformName(waveform_);
}
