#pragma once

#include "parser_callbacks.h"

#include "glib.h"

gchar *get_lsof_executable_path();
void set_lsof_executable_path(const gchar *path);
GError *lsof();
gboolean parse_lsof_output(const GByteArray *lsof_output,
                           ParserCallbacks *parser_callbacks);
