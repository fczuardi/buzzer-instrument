#pragma once

#include <cstdint>

#include "ToneWaveform.h"

// Hardware-neutral output used by instrument orchestration code.
//
// Implementations decide how MIDI note, waveform, and velocity become physical
// sound. Velocity follows the MIDI Note On range; each backend may map it to
// volume, timbre, articulation, or another hardware-specific behavior.
class VoiceOutput {
public:
  virtual ~VoiceOutput() = default;

  virtual bool startNote(
      uint8_t midiNote,
      ToneWaveform waveform,
      uint8_t velocity) = 0;
  virtual void stopNote() = 0;
  virtual bool isPlaying() const = 0;
};
