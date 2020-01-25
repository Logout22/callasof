#pragma once

#include "foreach.h"

#define STATE_POINTER(p) p##_state,
#define STATE_DEFINITIONS(...)                                                 \
  static state_fn state[] = {FOR_EACH(STATE_POINTER, __VA_ARGS__)};            \
  enum state_codes { __VA_ARGS__ };

#define LOOKUP_TRANSITION(current_state, transition_event)                     \
  (state_transition_table[current_state][transition_event])
