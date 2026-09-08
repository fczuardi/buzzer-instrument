#include "M5BuzzerToneOutput.h"

#include <M5Unified.h>

M5BuzzerToneOutput::M5BuzzerToneOutput()
    : core_(
          SPEAKER_VOLUME,
          {MIN_VELOCITY_VOLUME, MAX_VELOCITY_VOLUME},
          ToneWaveform::Saw32) {
}

void M5BuzzerToneOutput::begin() {
  auto config = M5.Speaker.config();
  config.buzzer = true;
  config.pin_data_out = BUZZER_GPIO_PIN;
  config.sample_rate = SPEAKER_SAMPLE_RATE_HZ;
  config.magnification = SPEAKER_MAGNIFICATION;
  M5.Speaker.config(config);

  core_.begin();
}

void M5BuzzerToneOutput::end() {
  core_.end();
}

bool M5BuzzerToneOutput::startNote(
    uint8_t midiNote,
    float frequencyHz,
    ToneWaveform waveform,
    uint8_t velocity) {
  return core_.startNote(midiNote, frequencyHz, waveform, velocity);
}

void M5BuzzerToneOutput::stopNote() {
  core_.stopNote();
}

bool M5BuzzerToneOutput::isPlaying() const {
  return core_.isPlaying();
}

bool M5BuzzerToneOutput::startTone(float frequencyHz) {
  return core_.startTone(frequencyHz);
}

void M5BuzzerToneOutput::stop() {
  core_.stop();
}

void M5BuzzerToneOutput::setVolume(uint8_t volume) {
  core_.setVolume(volume);
}

uint8_t M5BuzzerToneOutput::volume() const {
  return core_.volume();
}

void M5BuzzerToneOutput::setVelocityVolumeRange(VelocityVolumeRange range) {
  core_.setVelocityVolumeRange(range);
}

VelocityVolumeRange M5BuzzerToneOutput::velocityVolumeRange() const {
  return core_.velocityVolumeRange();
}

uint8_t M5BuzzerToneOutput::volumeForVelocity(uint8_t velocity) const {
  return core_.volumeForVelocity(velocity);
}

void M5BuzzerToneOutput::setWaveform(ToneWaveform waveform) {
  core_.setWaveform(waveform);
}

ToneWaveform M5BuzzerToneOutput::waveform() const {
  return core_.waveform();
}

const char* M5BuzzerToneOutput::waveformName() const {
  return core_.waveformName();
}
