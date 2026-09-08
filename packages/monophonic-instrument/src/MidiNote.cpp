#include "MidiNote.h"

#include <cstdio>
#include <cmath>

namespace {
constexpr uint8_t A4_MIDI_NOTE = 69;
constexpr float A4_FREQUENCY_HZ = 440.0f;
constexpr float SEMITONES_PER_OCTAVE = 12.0f;

constexpr const char* NOTE_NAMES[] = {
    "C",
    "C#",
    "D",
    "D#",
    "E",
    "F",
    "F#",
    "G",
    "G#",
    "A",
    "A#",
    "B",
};
}

float midiNoteToFrequencyHz(uint8_t midiNoteNumber) {
  const int semitoneOffset =
      static_cast<int>(midiNoteNumber) - static_cast<int>(A4_MIDI_NOTE);
  return A4_FREQUENCY_HZ *
         std::pow(2.0f, static_cast<float>(semitoneOffset) / SEMITONES_PER_OCTAVE);
}

void midiNoteName(uint8_t midiNoteNumber, char* output, size_t outputSize) {
  if (outputSize == 0) {
    return;
  }

  const uint8_t pitchClass = midiNoteNumber % 12;
  const int octave = static_cast<int>(midiNoteNumber / 12) - 1;
  std::snprintf(output, outputSize, "%s%d", NOTE_NAMES[pitchClass], octave);
}
