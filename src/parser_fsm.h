#pragma once

#include "fsm.h"

#include <stdio.h>

static int fsm_entry_state(void);
static int foo_state(void);
static int bar_state(void);
static int fsm_exit_state(void);

STATE_DEFINITIONS(fsm_entry, foo, bar, fsm_exit)

enum transition_events { ok, fail, repeat };

struct transition state_transitions[] = {
    {fsm_entry, ok, foo},  {fsm_entry, fail, fsm_exit}, {foo, ok, bar},
    {foo, fail, fsm_exit}, {foo, repeat, foo},          {bar, ok, fsm_exit},
    {bar, fail, fsm_exit}, {bar, repeat, foo}};

static int fsm_entry_state(void) {
  fprintf(stderr, "entry\n");
  return ok;
}

static int foo_state(void) {
  fprintf(stderr, "foo\n");
  return ok;
}

static int bar_state(void) {
  fprintf(stderr, "bar\n");
  return repeat;
}

static int fsm_exit_state(void) {
  fprintf(stderr, "exit\n");
  return ok;
}