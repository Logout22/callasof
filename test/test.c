#include "callasof/callasof.h"

static void
test_fails_if_lsof_cannot_be_found ()
{
    set_lsof_executable_path("/current_dir/no_lsof");
    GError* error = lsof();
    g_assert_true(error);
    g_assert_error(error, NULL, NULL);
    g_error_free (error);
}

int main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);

    // Define the tests.
    g_test_add_func(
            "/my-object/test_fails_if_lsof_cannot_be_found",
            test_fails_if_lsof_cannot_be_found);

    return g_test_run ();
}
