#pragma once

#include <cstddef>
#include <cstdint>

// Converts MIDI note numbers to equal-tempered frequencies.
//
// MIDI note 69 is A4, defined as 440 Hz. Each semitone changes frequency by the
// twelfth root of two.
float midiNoteToFrequencyHz(uint8_t midiNoteNumber);

// Writes a compact note name such as C0, C#4, or G9 into the caller's buffer.
// The project uses the common convention where MIDI note 60 is C4.
void midiNoteName(uint8_t midiNoteNumber, char* output, size_t outputSize);
