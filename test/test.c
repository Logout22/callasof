#include <glib.h>

typedef struct {
} CallasofFixture;

static void
callasof_fixture_set_up (CallasofFixture *fixture,
                          gconstpointer user_data)
{
    (void)fixture;(void)user_data;
}

static void
callasof_fixture_tear_down (CallasofFixture *fixture,
                             gconstpointer user_data)
{
    (void)fixture;(void)user_data;
}

static void
test_fails_if_lsof_cannot_be_found (CallasofFixture *fixture,
        gconstpointer user_data)
{
    (void)fixture;(void)user_data;
}

int main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);

    // Define the tests.
    g_test_add ("/my-object/test_fails_if_lsof_cannot_be_found", CallasofFixture, NULL,
            callasof_fixture_set_up, test_fails_if_lsof_cannot_be_found,
            callasof_fixture_tear_down);

    return g_test_run ();
}
