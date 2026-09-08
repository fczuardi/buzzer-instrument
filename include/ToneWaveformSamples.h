#pragma once

#include <cstddef>
#include <cstdint>

#include "ToneWaveform.h"

struct ToneWaveformSamples {
  const uint8_t* samples;
  size_t sampleCount;
};

ToneWaveformSamples toneWaveformSamples(ToneWaveform waveform);
