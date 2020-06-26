#pragma once

#include "glib.h"

typedef struct ParserCallbacks {
  void (*on_record_parsed)(GPtrArray *process_records, GHashTable *record);
  void (*on_process_parsed)(GPtrArray *process_records);
} ParserCallbacks;
