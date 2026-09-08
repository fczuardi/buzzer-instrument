#include "ToneWaveformSamples.h"

namespace {
struct ToneWaveformDefinition {
  ToneWaveform waveform;
  ToneWaveformSamples samples;
};

constexpr uint8_t SQUARE_WAVE_32[] = {
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr uint8_t SAW_WAVE_32[] = {
    0,   8,   16,  25,  33,  41,  49,  58,
    66,  74,  82,  90,  99,  107, 115, 123,
    132, 140, 148, 156, 165, 173, 181, 189,
    197, 206, 214, 222, 230, 239, 247, 255,
};

constexpr ToneWaveformDefinition WAVEFORMS[] = {
    {ToneWaveform::Square32, {SQUARE_WAVE_32, sizeof(SQUARE_WAVE_32)}},
    {ToneWaveform::Saw32, {SAW_WAVE_32, sizeof(SAW_WAVE_32)}},
};
}

ToneWaveformSamples toneWaveformSamples(ToneWaveform waveform) {
  for (const ToneWaveformDefinition& definition : WAVEFORMS) {
    if (definition.waveform == waveform) {
      return definition.samples;
    }
  }

  return WAVEFORMS[0].samples;
}
