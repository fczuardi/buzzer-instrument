#include "SpeakerToneOutput.h"
#include "ToneWaveformSamples.h"

#include <Arduino.h>
#include <M5Unified.h>

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

  const ToneWaveformSamples waveformSamples = toneWaveformSamples(waveform_);
  const bool toneStarted = M5.Speaker.tone(
      frequencyHz,
      UINT32_MAX,
      SPEAKER_CHANNEL,
      true,
      waveformSamples.samples,
      waveformSamples.sampleCount);

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
