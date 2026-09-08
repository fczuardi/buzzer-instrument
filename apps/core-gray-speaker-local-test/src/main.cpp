#include <Arduino.h>
#include <M5Unified.h>

#include "M5CoreGrayToneOutput.h"
#include "MidiNote.h"
#include "ToneWaveform.h"

namespace {
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;
constexpr uint8_t DEFAULT_TEST_VELOCITY = 96;
constexpr uint8_t MIDI_NOTE_C3 = 48;
constexpr uint8_t MIDI_NOTE_C4 = 60;
constexpr uint8_t MIDI_NOTE_A4 = 69;
constexpr uint8_t MIDI_NOTE_C5 = 72;
constexpr uint8_t MIDI_NOTE_C6 = 84;
constexpr size_t NOTE_NAME_BUFFER_SIZE = 8;

constexpr uint8_t SWEEP_NOTES[] = {
    MIDI_NOTE_C3,
    MIDI_NOTE_C4,
    MIDI_NOTE_A4,
    MIDI_NOTE_C5,
    MIDI_NOTE_C6,
};

constexpr ToneWaveform WAVEFORMS[] = {
    ToneWaveform::Square32,
    ToneWaveform::Saw32,
};

M5CoreGrayToneOutput speakerOutput;
size_t selectedNoteIndex = 2;
size_t selectedWaveformIndex = 0;
uint32_t lastUptimeLogAtMs = 0;
uint32_t toneAttemptCount = 0;
bool lastToneStarted = false;

uint8_t currentMidiNote() {
  return SWEEP_NOTES[selectedNoteIndex];
}

float currentFrequencyHz() {
  return midiNoteToFrequencyHz(currentMidiNote());
}

void currentNoteName(char* output, size_t outputSize) {
  midiNoteName(currentMidiNote(), output, outputSize);
}

ToneWaveform currentWaveform() {
  return WAVEFORMS[selectedWaveformIndex];
}

bool tonePlaying() {
  return speakerOutput.isPlaying();
}

void drawScreen(const char* stateLabel) {
  char noteName[NOTE_NAME_BUFFER_SIZE] = {};
  currentNoteName(noteName, sizeof(noteName));

  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("Core Gray");
  M5.Display.println("note sweep");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println();
  M5.Display.printf("Board id: %d\n", static_cast<int>(M5.getBoard()));
  M5.Display.printf("Note: %s\n", noteName);
  M5.Display.printf("Freq: %.2f Hz\n", currentFrequencyHz());
  M5.Display.printf("Wave: %s\n", toneWaveformName(currentWaveform()));
  M5.Display.printf("Volume: %u\n", speakerOutput.volume());
  M5.Display.printf("Tone attempts: %lu\n", toneAttemptCount);
  M5.Display.print("Output: ");
  M5.Display.println(tonePlaying() ? "playing" : "stopped");
  M5.Display.print("Last request: ");
  M5.Display.println(lastToneStarted ? "accepted" : "none/failed");
  M5.Display.print("State: ");
  M5.Display.println(stateLabel);
  M5.Display.println();
  M5.Display.println("BtnA: start/stop");
  M5.Display.println("BtnB: next note");
  M5.Display.println("BtnC: waveform");
}

void logSpeakerSetup() {
  Serial.println();
  Serial.println("M5Stack Core Gray local note sweep");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.printf(
      "speaker: backend=m5_core_gray volume=%u notes=%u waveforms=%u\n",
      speakerOutput.volume(),
      static_cast<unsigned>(sizeof(SWEEP_NOTES) / sizeof(SWEEP_NOTES[0])),
      static_cast<unsigned>(sizeof(WAVEFORMS) / sizeof(WAVEFORMS[0])));
}

void startTone(const char* reason) {
  char noteName[NOTE_NAME_BUFFER_SIZE] = {};
  currentNoteName(noteName, sizeof(noteName));

  toneAttemptCount++;
  lastToneStarted = speakerOutput.startNote(
      currentMidiNote(),
      currentFrequencyHz(),
      currentWaveform(),
      DEFAULT_TEST_VELOCITY);

  Serial.printf(
      "speaker: tone_start reason=%s ok=%s note=%s midi_note=%u frequency_hz=%.2f waveform=%s volume=%u\n",
      reason,
      lastToneStarted ? "true" : "false",
      noteName,
      currentMidiNote(),
      currentFrequencyHz(),
      toneWaveformName(currentWaveform()),
      speakerOutput.volume());
  drawScreen(lastToneStarted ? "tone requested" : "tone failed");
}

void stopTone(const char* reason) {
  if (!speakerOutput.isPlaying()) {
    return;
  }

  speakerOutput.stopNote();

  Serial.printf("speaker: tone_stop reason=%s\n", reason);
  drawScreen("stopped");
}

void toggleTone() {
  if (speakerOutput.isPlaying()) {
    stopTone("button_a");
    return;
  }

  startTone("button_a");
}

bool restartToneIfPlaying(const char* reason) {
  if (!speakerOutput.isPlaying()) {
    return false;
  }

  speakerOutput.stopNote();
  startTone(reason);
  return true;
}

void selectNextNote() {
  char noteName[NOTE_NAME_BUFFER_SIZE] = {};

  selectedNoteIndex =
      (selectedNoteIndex + 1) % (sizeof(SWEEP_NOTES) / sizeof(SWEEP_NOTES[0]));
  currentNoteName(noteName, sizeof(noteName));

  Serial.printf(
      "speaker: note_select note=%s frequency_hz=%.2f\n",
      noteName,
      currentFrequencyHz());
  if (!restartToneIfPlaying("note_change")) {
    drawScreen("note selected");
  }
}

void selectNextWaveform() {
  selectedWaveformIndex =
      (selectedWaveformIndex + 1) % (sizeof(WAVEFORMS) / sizeof(WAVEFORMS[0]));

  Serial.printf(
      "speaker: waveform_select waveform=%s\n",
      toneWaveformName(currentWaveform()));
  if (!restartToneIfPlaying("waveform_change")) {
    drawScreen("waveform selected");
  }
}

void logUptime(uint32_t nowMs) {
  if (nowMs - lastUptimeLogAtMs < UPTIME_LOG_INTERVAL_MS) {
    return;
  }

  char noteName[NOTE_NAME_BUFFER_SIZE] = {};
  currentNoteName(noteName, sizeof(noteName));

  lastUptimeLogAtMs = nowMs;
  Serial.printf(
      "speaker: uptime_ms=%lu board_id=%d note=%s midi_note=%u frequency_hz=%.2f waveform=%s volume=%u playing=%s tone_attempts=%lu\n",
      nowMs,
      static_cast<int>(M5.getBoard()),
      noteName,
      currentMidiNote(),
      currentFrequencyHz(),
      toneWaveformName(currentWaveform()),
      speakerOutput.volume(),
      speakerOutput.isPlaying() ? "true" : "false",
      toneAttemptCount);
}
}

void setup() {
  auto config = M5.config();
  config.internal_spk = true;
  config.internal_mic = false;
  M5.begin(config);

  Serial.begin(SERIAL_BAUD);
  delay(200);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(96);

  speakerOutput.begin();

  logSpeakerSetup();
  drawScreen("ready");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    toggleTone();
  }

  if (M5.BtnB.wasPressed()) {
    selectNextNote();
  }

  if (M5.BtnC.wasPressed()) {
    selectNextWaveform();
  }

  logUptime(millis());
  delay(10);
}
