#pragma once

#include <cstdint>

// Drives the buzzer through M5Unified's Speaker abstraction.
//
// This class owns only hardware output. Musical decisions such as note names,
// note priority, or MIDI behavior should live in higher-level code.
class SpeakerToneOutput {
public:
  enum class Waveform : uint8_t {
    Square32,
    Saw32,
  };

  void begin();
  void end();
  bool startTone(float frequencyHz);
  void stop();
  bool isPlaying() const;
  void setWaveform(Waveform waveform);
  Waveform waveform() const;
  const char* waveformName() const;

private:
  static constexpr int SPEAKER_CHANNEL = 0;
  static constexpr uint8_t BUZZER_GPIO_PIN = 2;
  static constexpr uint8_t SPEAKER_VOLUME = 128;
  static constexpr uint8_t SPEAKER_MAGNIFICATION = 32;
  static constexpr uint32_t SPEAKER_SAMPLE_RATE_HZ = 48000;

  bool initialized_ = false;
  bool playing_ = false;
  Waveform waveform_ = Waveform::Saw32;
};
