#include "MonophonicInstrument.h"

int main() {
  MonophonicInstrument instrument;
  const VoiceAction action = instrument.noteOn(69, 100);
  return action.type == VoiceActionType::StartNote ? 0 : 1;
}
