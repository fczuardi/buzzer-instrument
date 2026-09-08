#pragma once

#include <cstdint>

#include "ToneWaveform.h"
#include "VoiceOutput.h"

struct VelocityVolumeRange {
  uint8_t minimum;
  uint8_t maximum;
};

// Drives the M5StickC Plus2 buzzer through M5Unified's Speaker abstraction.
//
// This class owns only hardware output. Musical decisions such as note names,
// note priority, or MIDI behavior should live in higher-level code.
class SpeakerToneOutput : public VoiceOutput {
public:
  void begin();
  void end();
  bool startNote(
      uint8_t midiNote,
      float frequencyHz,
      ToneWaveform waveform,
      uint8_t velocity) override;
  void stopNote() override;
  bool isPlaying() const override;
  bool startTone(float frequencyHz);
  void stop();
  void setVolume(uint8_t volume);
  uint8_t volume() const;
  void setVelocityVolumeRange(VelocityVolumeRange range);
  VelocityVolumeRange velocityVolumeRange() const;
  uint8_t volumeForVelocity(uint8_t velocity) const;
  void setWaveform(ToneWaveform waveform);
  ToneWaveform waveform() const;
  const char* waveformName() const;

private:
  static constexpr int SPEAKER_CHANNEL = 0;
  static constexpr uint8_t BUZZER_GPIO_PIN = 2;
  static constexpr uint8_t SPEAKER_VOLUME = 128;
  // Hardware tests found this constrained range clearer than the full 0..255
  // M5 speaker volume range, which was too quiet at the bottom and distorted
  // near the top.
  static constexpr uint8_t MIN_VELOCITY_VOLUME = 64;
  static constexpr uint8_t MAX_VELOCITY_VOLUME = 128;
  static constexpr uint8_t SPEAKER_MAGNIFICATION = 32;
  static constexpr uint32_t SPEAKER_SAMPLE_RATE_HZ = 48000;

  bool initialized_ = false;
  bool playing_ = false;
  uint8_t volume_ = SPEAKER_VOLUME;
  VelocityVolumeRange velocityVolumeRange_ = {
      MIN_VELOCITY_VOLUME,
      MAX_VELOCITY_VOLUME,
  };
  ToneWaveform waveform_ = ToneWaveform::Saw32;
};
