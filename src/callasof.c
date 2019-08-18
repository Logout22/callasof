#include "callasof/callasof.h"

#define max_path 65535

static gchar* _internal_lsof_executable_path() {
    static gchar lsof_executable_path[max_path];
    static gboolean path_initialized = FALSE;
    if (!path_initialized) {
        strcpy(lsof_executable_path, "/usr/bin/lsof");
        path_initialized = TRUE;
    }
    return lsof_executable_path;
}

gchar* get_lsof_executable_path() {
    return _internal_lsof_executable_path();
}

void set_lsof_executable_path(const gchar* path) {
    gchar* internal_path = _internal_lsof_executable_path();
    g_strlcpy(internal_path, path, max_path);
}

GError* lsof()
{
    gchar* argv[2];
    argv[0] = get_lsof_executable_path();
    argv[1] = NULL;
    GError* spawn_error = NULL;
    if (!g_spawn_sync(
                NULL,
                argv, NULL,
                G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                NULL, NULL,
                NULL, NULL,
                NULL, &spawn_error)) {
        return spawn_error;
    }
    return NULL;
}
