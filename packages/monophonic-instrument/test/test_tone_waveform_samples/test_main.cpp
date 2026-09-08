#include <unity.h>

#include "ToneWaveformSamples.h"

void test_square32_samples_are_available() {
  const ToneWaveformSamples samples =
      toneWaveformSamples(ToneWaveform::Square32);

  TEST_ASSERT_NOT_NULL(samples.samples);
  TEST_ASSERT_EQUAL_UINT(32, samples.sampleCount);
  TEST_ASSERT_EQUAL_UINT8(255, samples.samples[0]);
  TEST_ASSERT_EQUAL_UINT8(0, samples.samples[16]);
}

void test_saw32_samples_are_available() {
  const ToneWaveformSamples samples =
      toneWaveformSamples(ToneWaveform::Saw32);

  TEST_ASSERT_NOT_NULL(samples.samples);
  TEST_ASSERT_EQUAL_UINT(32, samples.sampleCount);
  TEST_ASSERT_EQUAL_UINT8(0, samples.samples[0]);
  TEST_ASSERT_EQUAL_UINT8(255, samples.samples[31]);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_square32_samples_are_available);
  RUN_TEST(test_saw32_samples_are_available);
  return UNITY_END();
}
