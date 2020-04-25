#include "parser_fsm.h"

#include "callasof/callasof.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define max_path 65535

static gchar *_internal_lsof_executable_path() {
  static gchar lsof_executable_path[max_path];
  static gboolean path_initialized = FALSE;
  if (!path_initialized) {
    strcpy(lsof_executable_path, "/usr/bin/lsof");
    path_initialized = TRUE;
  }
  return lsof_executable_path;
}

gchar *get_lsof_executable_path() { return _internal_lsof_executable_path(); }

void set_lsof_executable_path(const gchar *path) {
  gchar *internal_path = _internal_lsof_executable_path();
  g_strlcpy(internal_path, path, max_path);
}

GError *lsof() {
  gchar *argv[2];
  argv[0] = get_lsof_executable_path();
  argv[1] = NULL;
  GError *spawn_error = NULL;
  if (!g_spawn_sync(NULL, argv, NULL,
                    G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                    NULL, NULL, NULL, NULL, NULL, &spawn_error)) {
    return spawn_error;
  }
  return NULL;
}

static gboolean process_character(ParserFsmState *context,
                                  gchar current_character);

gboolean parse_lsof_output(const GByteArray *lsof_output,
                           ParserCallbacks *parser_callbacks) {
  ParserFsmState context;
  context.current_content = g_string_new("");
  context.current_record =
      g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
  context.current_state = started_parsing;
  context.parser_callbacks = parser_callbacks;
  for (gsize i = 0; i <= lsof_output->len; ++i) {
    gchar current_character =
        i < lsof_output->len ? g_array_index(lsof_output, gchar, i) : 0;
    gboolean processing_successful =
        process_character(&context, current_character);
    if (!processing_successful) {
      fprintf(stderr,
              "ERROR: Could not parse character (see previous errors)\n");
      return FALSE;
    }
  }
  g_string_free(context.current_content, TRUE);
  return TRUE;
}

static gboolean process_character(ParserFsmState *context,
                                  gchar current_character) {
  state_fn state_function = state[context->current_state];
  enum transition_events event = state_function(context, current_character);
  enum state_codes next_state =
      LOOKUP_TRANSITION(context->current_state, event);
  if (next_state == invalid) {
    fprintf(stderr, "ERROR: No transition from %d with event %d\n",
            context->current_state, event);
    return FALSE;
  }
  context->current_state = next_state;
  return TRUE;
}
