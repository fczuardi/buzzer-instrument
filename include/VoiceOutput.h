#pragma once

#include <cstdint>

#include "ToneWaveform.h"

// Hardware-neutral output used by instrument orchestration code.
//
// Implementations decide how a MIDI note and waveform become physical sound.
class VoiceOutput {
public:
  virtual ~VoiceOutput() = default;

  virtual bool startNote(uint8_t midiNote, ToneWaveform waveform) = 0;
  virtual void stopNote() = 0;
  virtual bool isPlaying() const = 0;
};
