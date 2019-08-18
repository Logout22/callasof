#include "callasof/callasof.h"

#define max_path 65535
static char lsof_executable_path[max_path];

void set_lsof_executable_path(const char* path) {
    g_strlcpy(lsof_executable_path, path, max_path);
}

GError* lsof()
{
    char* argv[2];
    argv[1] = lsof_executable_path;
    argv[2] = NULL;
    GError* spawnError = NULL;
    if (!g_spawn_sync(
                NULL,
                argv, NULL,
                G_SPAWN_DEFAULT,
                NULL, NULL,
                NULL, NULL,
                NULL, &spawnError)) {
        return spawnError;
    }
    return NULL;
}
