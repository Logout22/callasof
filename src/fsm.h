#pragma once

#include "foreach.h"

#define STATE_POINTER(p) p##_state,
#define STATE_DEFINITIONS(...)                                                 \
  static int (*state[])(void) = {FOR_EACH(STATE_POINTER, __VA_ARGS__)};        \
  enum state_codes { __VA_ARGS__ };

struct transition {
  unsigned source_state;
  unsigned event;
  unsigned destination_state;
};

// TODO: implement transition schema from this comment:
// https://stackoverflow.com/questions/1371460/state-machines-tutorials#comment28594011_1371654
static unsigned lookup_transitions(unsigned current_state,
                                   unsigned transition_event,
                                   struct transition *state_transitions,
                                   unsigned transition_count) {
  for (unsigned i = 0; i < transition_count; ++i) {
    if (state_transitions[i].source_state == current_state &&
        state_transitions[i].event == transition_event) {
      return state_transitions[i].destination_state;
    }
  }
  return -1;
}

#define EXIT_STATE fsm_exit
#define ENTRY_STATE fsm_entry
