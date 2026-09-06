#include <Arduino.h>
#include <M5Unified.h>

#include "MidiNote.h"
#include "MonophonicInstrument.h"
#include "MonophonicInstrumentSink.h"
#include "SpeakerToneOutput.h"

namespace {
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;
constexpr uint8_t DEFAULT_TEST_CHANNEL = 1;
constexpr uint8_t DEFAULT_TEST_VELOCITY = 100;

MonophonicInstrument instrument;
SpeakerToneOutput speakerToneOutput;
MonophonicInstrumentSink instrumentSink(instrument, speakerToneOutput);
bool tonePlaying = false;
uint32_t lastUptimeLogAtMs = 0;

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
  M5.Display.println("BtnB: waveform");
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
  M5.Display.printf(
      "Note: %s (%u)\n",
      noteName,
      midiNote);
  M5.Display.print("Freq: ");
  M5.Display.print(midiNoteToFrequencyHz(midiNote), 2);
  M5.Display.println(" Hz");
  M5.Display.print("State: ");
  M5.Display.println(stateLabel);
}

void drawInstrumentState(const char* stateLabel) {
  drawToneState(instrument.midiNoteNumber(), stateLabel);
}

void startNote(uint8_t midiNote) {
  char noteName[5];
  midiNoteName(midiNote, noteName, sizeof(noteName));

  const float frequencyHz = midiNoteToFrequencyHz(midiNote);
  const bool toneStarted =
      speakerToneOutput.startNote(midiNote, instrument.waveform());
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
  speakerToneOutput.setWaveform(instrument.waveform());

  Serial.println();
  Serial.println("M5StickC Plus2 buzzer instrument");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.printf(
      "buzzer: backend=m5speaker pin=2 volume=128 magnification=32 sample_rate_hz=48000 waveform=%s\n",
      speakerToneOutput.waveformName());

  drawStaticScreen();
  drawInstrumentState("idle");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    const NoteEvent event = {
        NoteEventType::NoteOn,
        DEFAULT_TEST_CHANNEL,
        MonophonicInstrument::DEFAULT_TEST_NOTE,
        DEFAULT_TEST_VELOCITY,
    };
    dispatchLocalNoteEvent(event);
  }

  if (M5.BtnA.wasReleased()) {
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

    instrument.selectNextWaveform();
    char noteName[5];
    instrument.noteName(noteName, sizeof(noteName));
    Serial.printf(
        "buzzer: waveform_selected waveform=%s note=%s midi_note=%u frequency_hz=%.2f\n",
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

  const uint32_t nowMs = millis();
  if (nowMs - lastUptimeLogAtMs >= UPTIME_LOG_INTERVAL_MS) {
    lastUptimeLogAtMs = nowMs;
    Serial.printf("uptime=%lu\n", nowMs / 1000);
  }
}
