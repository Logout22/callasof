#include "parser_fsm.h"

#include "callasof/callasof.h"

#include <assert.h>
#include <stdio.h>

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

GHashTable *parse_lsof_output(const GByteArray *lsof_output) {
  enum state_codes current_state = started_parsing;
  ParserFsmState context;
  context.current_content = g_string_new("");
  context.current_record =
      g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
  for (gsize i = 0; i < lsof_output->len; ++i) {
    gchar current_character = g_array_index(lsof_output, gchar, i);
    state_fn state_function = state[current_state];
    enum transition_events event = state_function(&context, current_character);
    enum state_codes next_state = LOOKUP_TRANSITION(current_state, event);
    if (next_state == invalid) {
      fprintf(stderr, "ERROR: No transition from %d with event %d\n",
              current_state, event);
      assert(FALSE);
      break;
    }
    current_state = next_state;
  }
  return context.current_record;
}
