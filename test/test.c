#include "callasof/callasof.h"

static void
test_fails_if_lsof_cannot_be_found ()
{
    set_lsof_executable_path("/current_dir/no_lsof");
    GError* error = lsof();
    g_assert_true(error);
    g_assert_error(error, G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT);
    g_error_free (error);
}

static void
test_lsof_path_initialized_to_default ()
{
    gchar* lsof_path = get_lsof_executable_path();
    g_assert_cmpuint(strlen(lsof_path), >, 0);
}

int main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);

    // Define the tests.
    g_test_add_func(
            "/callasof/test_fails_if_lsof_cannot_be_found",
            test_fails_if_lsof_cannot_be_found);
    g_test_add_func(
            "/callasof/test_lsof_path_initialized_to_default",
            test_lsof_path_initialized_to_default);

    return g_test_run ();
}
