#include "../src/parser_fsm.h"

#include "parser_test.h"

void test_parser() {
  enum state_codes cur_state = ENTRY_STATE;
  enum transition_events rc;
  int (*state_fun)(void);

  for (;;) {
    state_fun = state[cur_state];
    rc = state_fun();
    if (EXIT_STATE == cur_state)
      break;
    cur_state = lookup_transitions(cur_state, rc, state_transitions,
                                   sizeof(state_transitions));
  }
}