#pragma once

#include "fsm.h"

#include "callasof/callasof.h"

#include "glib.h"

typedef struct ParserFsmState {
  guint current_state;
  gchar current_byte;
  gchar current_identifier;
  GString *current_content;
  GHashTable *current_record;
  GPtrArray *current_process_records;
  ParserCallbacks *parser_callbacks;
} ParserFsmState;

typedef unsigned (*state_fn)(ParserFsmState *state, gchar next);

static unsigned started_parsing_state(ParserFsmState *state, gchar next);
static unsigned collecting_identifier_state(ParserFsmState *state, gchar next);
static unsigned collecting_content_state(ParserFsmState *state, gchar next);
static unsigned creating_field_state(ParserFsmState *state, gchar next);
static unsigned creating_record_state(ParserFsmState *state, gchar next);
static unsigned invalid_state(ParserFsmState *state, gchar next) {
  (void)state;
  (void)next;
  return 0;
}

STATE_DEFINITIONS(started_parsing, collecting_identifier, collecting_content,
                  creating_field, creating_record, invalid)

enum transition_events {
  zero,
  newline,
  other_character,
  transition_events_count
};

// clang-format off
// transitions for events in the order as above
enum state_codes state_transition_table[][transition_events_count] = {
    {collecting_identifier, collecting_identifier, collecting_identifier}, // started_parsing
    {creating_field       , invalid              , collecting_content   }, // collecting_identifier
    {creating_field       , invalid              , collecting_content   }, // collecting_content
    {invalid              , creating_record      , collecting_identifier}, // creating_field
    {creating_field       , invalid              , collecting_identifier}, // creating_record
};
// clang-format on

static enum transition_events determine_event(gchar input) {
  switch (input) {
  case '\0':
    return zero;
  case '\n':
    return newline;
  default:
    return other_character;
  }
}

static enum transition_events shift_byte_and_event(ParserFsmState *state,
                                                   gchar next) {
  state->current_byte = next;
  return determine_event(next);
}

static enum transition_events started_parsing_state(ParserFsmState *state,
                                                    gchar next) {
  return shift_byte_and_event(state, next);
}

static void check_and_notify_process_entry(ParserFsmState *state);
static void g_hash_table_destroy_with_loose_signature(void *hash_table);

static enum transition_events collecting_identifier_state(ParserFsmState *state,
                                                          gchar next) {
  state->current_identifier = state->current_byte;
  check_and_notify_process_entry(state);
  return shift_byte_and_event(state, next);
}

static void check_and_notify_process_entry(ParserFsmState *state) {
  if (state->current_identifier == 'p') {
    if (state->parser_callbacks && state->parser_callbacks->on_process_parsed) {
      state->parser_callbacks->on_process_parsed(
          state->current_process_records);
    }
    state->current_process_records = g_ptr_array_new_with_free_func(
        g_hash_table_destroy_with_loose_signature);
  }
}

static void g_hash_table_destroy_with_loose_signature(void *hash_table) {
  g_hash_table_destroy((GHashTable *)hash_table);
}

static enum transition_events collecting_content_state(ParserFsmState *state,
                                                       gchar next) {
  g_string_append_c(state->current_content, state->current_byte);
  return shift_byte_and_event(state, next);
}

static enum transition_events creating_field_state(ParserFsmState *state,
                                                   gchar next) {
  gchar *raw_string = g_string_free(state->current_content, FALSE);
  g_hash_table_insert(state->current_record,
                      GINT_TO_POINTER(state->current_identifier), raw_string);
  state->current_content = g_string_new("");
  return shift_byte_and_event(state, next);
}

static enum transition_events creating_record_state(ParserFsmState *state,
                                                    gchar next) {
  if (state->parser_callbacks) {
    if (state->parser_callbacks->on_record_parsed) {
      state->parser_callbacks->on_record_parsed(state->current_record);
    }
  }

  g_ptr_array_add(state->current_process_records, state->current_record);
  state->current_record =
      g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);

  return shift_byte_and_event(state, next);
}
