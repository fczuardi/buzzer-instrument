#pragma once

#include <cstdint>

enum class NoteEventType : uint8_t {
  NoteOn,
  NoteOff,
};

// Typed musical note event shared by producers and instrument policy.
//
// This contract intentionally contains no BLE, parser, display, or hardware
// details. Pitch bend and control change should use separate event types.
struct NoteEvent {
  NoteEventType type;
  uint8_t channel;
  uint8_t note;
  uint8_t velocity;
};
