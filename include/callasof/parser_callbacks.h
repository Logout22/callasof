#pragma once

#include "glib.h"

typedef struct ParserCallbacks {
  void (*on_record_parsed)(GHashTable *record);
} ParserCallbacks;
