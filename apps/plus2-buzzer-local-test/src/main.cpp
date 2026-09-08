#include <Arduino.h>
#include <M5Unified.h>

#include "MidiNote.h"
#include "MonophonicInstrument.h"
#include "MonophonicInstrumentSink.h"
#include "M5BuzzerToneOutput.h"

namespace {
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;
constexpr uint32_t PITCH_BEND_DEMO_HOLD_MS = 1000;
constexpr uint32_t PITCH_BEND_DEMO_STEP_MS = 1000;
constexpr uint8_t DEFAULT_TEST_CHANNEL = 1;
constexpr uint8_t TEST_VELOCITY_LEVELS[] = {1, 32, 64, 96, 127};
constexpr VelocityVolumeRange TEST_VELOCITY_VOLUME_RANGE = {64, 128};
constexpr int16_t PITCH_BEND_CENTER = 0;
constexpr int16_t PITCH_BEND_MINIMUM = -8192;
constexpr int16_t PITCH_BEND_MAXIMUM = 8191;

MonophonicInstrument instrument;
M5BuzzerToneOutput speakerToneOutput;
MonophonicInstrumentSink instrumentSink(instrument, speakerToneOutput);
bool tonePlaying = false;
size_t selectedVelocityIndex = 3;
uint32_t lastUptimeLogAtMs = 0;
uint32_t buttonAPressedAtMs = 0;
uint32_t lastPitchBendDemoStepAtMs = 0;
uint8_t pitchBendDemoStep = 0;
bool pitchBendDemoActive = false;

void drawStaticScreen() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("Buzzer");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println("MIDI-like note event test");
  M5.Display.println();
  M5.Display.println("BtnA: C4 note on/off");
  M5.Display.println("Hold A: pitch bend demo");
  M5.Display.println("BtnB: velocity");
}

void drawToneState(uint8_t midiNote, const char* stateLabel) {
  constexpr int32_t STATE_ROW_Y = 72;
  constexpr int32_t STATE_ROW_HEIGHT = 48;

  M5.Display.fillRect(
      0,
      STATE_ROW_Y,
      M5.Display.width(),
      STATE_ROW_HEIGHT,
      TFT_BLACK);
  M5.Display.setCursor(0, STATE_ROW_Y);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  char noteName[5];
  midiNoteName(midiNote, noteName, sizeof(noteName));

  M5.Display.println("Output: M5.Speaker");
  M5.Display.print("Wave: ");
  M5.Display.println(instrument.waveformName());
  M5.Display.print("Vel: ");
  M5.Display.println(TEST_VELOCITY_LEVELS[selectedVelocityIndex]);
  M5.Display.print("Vol: ");
  M5.Display.println(
      speakerToneOutput.volumeForVelocity(
          TEST_VELOCITY_LEVELS[selectedVelocityIndex]));
  M5.Display.printf(
      "Note: %s (%u)\n",
      noteName,
      midiNote);
  M5.Display.print("Freq: ");
  M5.Display.print(instrument.frequencyHz(), 2);
  M5.Display.println(" Hz");
  M5.Display.print("Bend: ");
  M5.Display.println(instrument.pitchBendValue());
  M5.Display.print("State: ");
  M5.Display.println(stateLabel);
}

void drawInstrumentState(const char* stateLabel) {
  drawToneState(instrument.midiNoteNumber(), stateLabel);
}

void startNote(uint8_t midiNote) {
  char noteName[5];
  midiNoteName(midiNote, noteName, sizeof(noteName));

  const float frequencyHz = instrument.frequencyHz();
  const bool toneStarted =
      speakerToneOutput.startNote(
          midiNote,
          frequencyHz,
          instrument.waveform(),
          TEST_VELOCITY_LEVELS[selectedVelocityIndex]);
  tonePlaying = toneStarted;

  Serial.printf(
      "buzzer: tone_start ok=%s backend=m5speaker waveform=%s note=%s midi_note=%u frequency_hz=%.2f\n",
      toneStarted ? "true" : "false",
      speakerToneOutput.waveformName(),
      noteName,
      midiNote,
      frequencyHz);
  drawToneState(midiNote, toneStarted ? "playing" : "failed");
}

void stopTone(const char* reason) {
  if (!tonePlaying) {
    return;
  }

  speakerToneOutput.stopNote();
  tonePlaying = false;

  Serial.print("buzzer: tone_stop reason=");
  Serial.println(reason);
  drawInstrumentState("idle");
}

void dispatchLocalNoteEvent(const NoteEvent& event) {
  instrumentSink.onNoteEvent(event);
  tonePlaying = speakerToneOutput.isPlaying();

  Serial.printf(
      "buzzer: note_event type=%s channel=%u note=%u velocity=%u playing=%s\n",
      event.type == NoteEventType::NoteOn ? "note_on" : "note_off",
      event.channel,
      event.note,
      event.velocity,
      tonePlaying ? "true" : "false");

  if (tonePlaying) {
    drawToneState(instrument.midiNoteNumber(), "playing");
  } else {
    drawInstrumentState("idle");
  }
}

void dispatchLocalPitchBendEvent(int16_t bendValue) {
  const PitchBendEvent event = {
      DEFAULT_TEST_CHANNEL,
      bendValue,
  };
  instrumentSink.onPitchBendEvent(event);
  tonePlaying = speakerToneOutput.isPlaying();

  Serial.printf(
      "buzzer: pitch_bend channel=%u value=%d frequency_hz=%.2f playing=%s\n",
      event.channel,
      event.value,
      instrument.frequencyHz(),
      tonePlaying ? "true" : "false");

  if (tonePlaying) {
    drawToneState(instrument.midiNoteNumber(), "playing");
  } else {
    drawInstrumentState("idle");
  }
}

void resetPitchBendDemo() {
  pitchBendDemoActive = false;
  pitchBendDemoStep = 0;
  lastPitchBendDemoStepAtMs = 0;
}

void startPitchBendDemo(uint32_t nowMs) {
  pitchBendDemoActive = true;
  pitchBendDemoStep = 0;
  lastPitchBendDemoStepAtMs = nowMs;

  Serial.println("buzzer: pitch_bend_demo started");
  dispatchLocalPitchBendEvent(PITCH_BEND_CENTER);
}

void advancePitchBendDemo(uint32_t nowMs) {
  if (!pitchBendDemoActive ||
      nowMs - lastPitchBendDemoStepAtMs < PITCH_BEND_DEMO_STEP_MS) {
    return;
  }

  lastPitchBendDemoStepAtMs = nowMs;
  pitchBendDemoStep++;

  switch (pitchBendDemoStep) {
    case 1:
      dispatchLocalPitchBendEvent(PITCH_BEND_MINIMUM);
      break;
    case 2:
      dispatchLocalPitchBendEvent(PITCH_BEND_CENTER);
      break;
    case 3:
      dispatchLocalPitchBendEvent(PITCH_BEND_MAXIMUM);
      break;
    case 4:
      dispatchLocalPitchBendEvent(PITCH_BEND_CENTER);
      Serial.println("buzzer: pitch_bend_demo finished");
      resetPitchBendDemo();
      break;
    default:
      resetPitchBendDemo();
      break;
  }
}
}

void setup() {
  auto config = M5.config();
  config.internal_spk = true;
  config.internal_mic = false;
  config.fallback_board = m5::board_t::board_M5StickCPlus2;
  M5.begin(config);

  Serial.begin(115200);
  delay(200);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(96);

  speakerToneOutput.begin();
  speakerToneOutput.setVelocityVolumeRange(TEST_VELOCITY_VOLUME_RANGE);
  speakerToneOutput.setVolume(
      speakerToneOutput.volumeForVelocity(
          TEST_VELOCITY_LEVELS[selectedVelocityIndex]));
  speakerToneOutput.setWaveform(instrument.waveform());

  Serial.println();
  Serial.println("M5StickC Plus2 buzzer instrument");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.printf(
      "buzzer: backend=m5speaker pin=2 volume=%u magnification=32 sample_rate_hz=48000 waveform=%s\n",
      speakerToneOutput.volume(),
      speakerToneOutput.waveformName());

  drawStaticScreen();
  drawInstrumentState("idle");
}

void loop() {
  M5.update();
  const uint32_t nowMs = millis();

  if (M5.BtnA.wasPressed()) {
    buttonAPressedAtMs = nowMs;
    resetPitchBendDemo();
    const NoteEvent event = {
        NoteEventType::NoteOn,
        DEFAULT_TEST_CHANNEL,
        MonophonicInstrument::DEFAULT_TEST_NOTE,
        TEST_VELOCITY_LEVELS[selectedVelocityIndex],
    };
    dispatchLocalNoteEvent(event);
  }

  if (M5.BtnA.isHolding() &&
      tonePlaying &&
      !pitchBendDemoActive &&
      nowMs - buttonAPressedAtMs >= PITCH_BEND_DEMO_HOLD_MS) {
    startPitchBendDemo(nowMs);
  }

  if (M5.BtnA.wasReleased()) {
    resetPitchBendDemo();
    const NoteEvent event = {
        NoteEventType::NoteOff,
        DEFAULT_TEST_CHANNEL,
        MonophonicInstrument::DEFAULT_TEST_NOTE,
        0,
    };
    dispatchLocalNoteEvent(event);
  }

  if (M5.BtnB.wasClicked()) {
    const bool wasPlaying = tonePlaying;
    if (wasPlaying) {
      stopTone("next_waveform");
    }

    selectedVelocityIndex =
        (selectedVelocityIndex + 1) %
        (sizeof(TEST_VELOCITY_LEVELS) / sizeof(TEST_VELOCITY_LEVELS[0]));
    speakerToneOutput.setVolume(
        speakerToneOutput.volumeForVelocity(
            TEST_VELOCITY_LEVELS[selectedVelocityIndex]));
    char noteName[5];
    instrument.noteName(noteName, sizeof(noteName));
    Serial.printf(
        "buzzer: velocity_selected velocity=%u mapped_volume=%u waveform=%s note=%s midi_note=%u frequency_hz=%.2f\n",
        TEST_VELOCITY_LEVELS[selectedVelocityIndex],
        speakerToneOutput.volumeForVelocity(
            TEST_VELOCITY_LEVELS[selectedVelocityIndex]),
        instrument.waveformName(),
        noteName,
        instrument.midiNoteNumber(),
        instrument.frequencyHz());

    if (wasPlaying) {
      startNote(instrument.midiNoteNumber());
    } else {
      drawInstrumentState("idle");
    }
  }

  advancePitchBendDemo(nowMs);

  if (nowMs - lastUptimeLogAtMs >= UPTIME_LOG_INTERVAL_MS) {
    lastUptimeLogAtMs = nowMs;
    Serial.printf("uptime=%lu\n", nowMs / 1000);
  }
}
