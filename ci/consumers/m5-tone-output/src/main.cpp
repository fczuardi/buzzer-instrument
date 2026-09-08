#include <Arduino.h>
#include <M5Unified.h>

#include "M5BuzzerToneOutput.h"
#include "MonophonicInstrument.h"
#include "MonophonicInstrumentSink.h"

MonophonicInstrument instrument;
M5BuzzerToneOutput output;
MonophonicInstrumentSink sink(instrument, output);

void setup() {
  output.setWaveform(instrument.waveform());
  output.setVelocityVolumeRange({64, 128});
  sink.onNoteEvent({NoteEventType::NoteOn, 1, 60, 100});
  sink.onNoteEvent({NoteEventType::NoteOff, 1, 60, 0});
}

void loop() {
}
