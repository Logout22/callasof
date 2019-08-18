#include <glib.h>

gchar* get_lsof_executable_path();
void set_lsof_executable_path(const gchar* path);
GError* lsof();
GHashTable* parse_lsof_output(const gchar* lsof_output);
