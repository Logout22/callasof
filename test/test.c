#include "callasof/callasof.h"

static gchar example_lsof_output[] =
"COMMAND    PID  TID TASKCMD      USER   FD      TYPE             DEVICE  SIZE/OFF       NODE NAME\n"
"bash      5903                munzner  cwd       DIR              254,3      4096    3407873 /home/munzner\n"
"bash      5903                munzner  rtd       DIR              254,2      4096          2 /\n"
"bash      5903                munzner  txt       REG              254,2    903464     137621 /usr/bin/bash\n"
"bash      5903                munzner  mem       REG              254,2     51376     134179 /usr/lib/libnss_files-2.29.so\n"
"bash      5903                munzner  mem       REG              254,2   3373280     161460 /usr/lib/locale/locale-archive\n"
"bash      5903                munzner  mem       REG              254,2    449040     134912 /usr/lib/libncursesw.so.6.1\n"
"bash      5903                munzner  mem       REG              254,2   2133648     134119 /usr/lib/libc-2.29.so\n"
"bash      5903                munzner  mem       REG              254,2     14512     134139 /usr/lib/libdl-2.29.so\n"
"bash      5903                munzner  mem       REG              254,2    326392     137593 /usr/lib/libreadline.so.8.0\n"
"bash      5903                munzner  mem       REG              254,2    198088     134074 /usr/lib/ld-2.29.so\n"
"bash      5903                munzner    0u      CHR              136,3       0t0          6 /dev/pts/3\n"
"bash      5903                munzner    1u      CHR              136,3       0t0          6 /dev/pts/3\n"
"bash      5903                munzner    2u      CHR              136,3       0t0          6 /dev/pts/3\n"
"bash      5903                munzner  255u      CHR              136,3       0t0          6 /dev/pts/3\n"
"lsof      6188                munzner  cwd       DIR              254,3      4096    3407873 /home/munzner\n"
"lsof      6188                munzner  rtd       DIR              254,2      4096          2 /\n"
"lsof      6188                munzner  txt       REG              254,2    168256     181755 /usr/bin/lsof\n"
"lsof      6188                munzner  mem       REG              254,2   3373280     161460 /usr/lib/locale/locale-archive\n"
"lsof      6188                munzner  mem       REG              254,2   2133648     134119 /usr/lib/libc-2.29.so\n"
"lsof      6188                munzner  mem       REG              254,2    198088     134074 /usr/lib/ld-2.29.so\n"
"lsof      6188                munzner    0u      CHR              136,3       0t0          6 /dev/pts/3\n"
"lsof      6188                munzner    1w      REG               0,40         0     172103 /tmp/test-lsof-output.txt\n"
"lsof      6188                munzner    2u      CHR              136,3       0t0          6 /dev/pts/3\n"
"lsof      6188                munzner    3r      DIR                0,4         0          1 /proc\n"
"lsof      6188                munzner    4r      DIR                0,4         0     172105 /proc/6188/fd\n"
"lsof      6188                munzner    5w     FIFO               0,12       0t0     172110 pipe\n"
"lsof      6188                munzner    6r     FIFO               0,12       0t0     172111 pipe\n"
"lsof      6189                munzner  cwd       DIR              254,3      4096    3407873 /home/munzner\n"
"lsof      6189                munzner  rtd       DIR              254,2      4096          2 /\n"
"lsof      6189                munzner  txt       REG              254,2    168256     181755 /usr/bin/lsof\n"
"lsof      6189                munzner  mem       REG              254,2   3373280     161460 /usr/lib/locale/locale-archive\n"
"lsof      6189                munzner  mem       REG              254,2   2133648     134119 /usr/lib/libc-2.29.so\n"
"lsof      6189                munzner  mem       REG              254,2    198088     134074 /usr/lib/ld-2.29.so\n"
"lsof      6189                munzner    4r     FIFO               0,12       0t0     172110 pipe\n"
"lsof      6189                munzner    7w     FIFO               0,12       0t0     172111 pipe";

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

static void
test_provides_pid_map ()
{
    GHashTable* parsed_output = parse_lsof_output(example_lsof_output);
    g_assert_true(parsed_output);
    GList* keys = g_hash_table_get_keys(parsed_output);

    g_assert_true(keys);
    size_t pid_list_length = 3;
    gint process_ids[] = {5903, 6188, 6189};
    g_assert_cmpuint(g_list_length(keys), ==, pid_list_length);
    for (size_t i = 0; i < pid_list_length; ++i)
    {
        g_assert_cmpint(g_list_index(keys, GINT_TO_POINTER(process_ids[i])), !=, -1);
    }
    g_list_free(keys);
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
    g_test_add_func(
            "/callasof/test_provides_pid_map",
            test_provides_pid_map);

    return g_test_run ();
}
