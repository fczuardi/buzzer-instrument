#pragma once

#include <cstdint>

#include "M5ToneOutputCore.h"
#include "ToneWaveform.h"
#include "VoiceOutput.h"

// M5StickC Plus2 buzzer output backed by M5Unified's Speaker abstraction.
class M5BuzzerToneOutput : public VoiceOutput {
public:
  M5BuzzerToneOutput();

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
  static constexpr uint8_t BUZZER_GPIO_PIN = 2;
  static constexpr uint8_t SPEAKER_VOLUME = 128;
  static constexpr uint8_t MIN_VELOCITY_VOLUME = 64;
  static constexpr uint8_t MAX_VELOCITY_VOLUME = 128;
  static constexpr uint8_t SPEAKER_MAGNIFICATION = 32;
  static constexpr uint32_t SPEAKER_SAMPLE_RATE_HZ = 48000;

  M5ToneOutputCore core_;
};
