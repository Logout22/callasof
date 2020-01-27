#include "callasof/callasof.h"

#include "test_output.h"

// Parser tests
void test_reads_all_fields_in_a_record() {
  const gsize test_input_length =
      STATIC_BYTE_ARRAY_LENGTH(example_single_line_output);
  GByteArray *input_array = g_byte_array_sized_new(test_input_length);
  g_byte_array_append(input_array, (const guint8 *)example_single_line_output,
                      test_input_length);

  GHashTable *result = parse_lsof_output(input_array);

#define element_count 6
  g_assert_cmpuint(element_count, ==, g_hash_table_size(result));
  gchar *expected_output[element_count][2] = {{"p", "9097"},  {"g", "9097"},
                                              {"R", "23617"}, {"c", "zsh"},
                                              {"u", "1000"},  {"L", "logout"}};
  for (guint i = 0; i < element_count; ++i) {
    gint key = expected_output[i][0][0];
    g_assert_cmpstr(expected_output[i][1], ==,
                    (gchar *)g_hash_table_lookup(result, GINT_TO_POINTER(key)));
  }

  g_hash_table_destroy(result);
  g_byte_array_free(input_array, TRUE);
}

// Main tests
static void test_fails_if_lsof_cannot_be_found() {
  set_lsof_executable_path("/current_dir/no_lsof");
  GError *error = lsof();
  g_assert_true(error);
  g_assert_error(error, G_SPAWN_ERROR, G_SPAWN_ERROR_NOENT);
  g_error_free(error);
}

static void test_lsof_path_initialized_to_default() {
  gchar *lsof_path = get_lsof_executable_path();
  g_assert_cmpuint(strlen(lsof_path), >, 0);
}

static void test_provides_pid_map() {
  const gsize test_input_length = STATIC_BYTE_ARRAY_LENGTH(example_lsof_output);
  GByteArray *input_array = g_byte_array_sized_new(test_input_length);
  g_byte_array_append(input_array, (const guint8 *)example_lsof_output,
                      test_input_length);

  GHashTable *parsed_output = parse_lsof_output(input_array);
  g_assert_true(parsed_output);
  GList *keys = g_hash_table_get_keys(parsed_output);

  g_assert_true(keys);
  size_t pid_list_length = 3;
  gint process_ids[] = {5903, 6188, 6189};
  g_assert_cmpuint(g_list_length(keys), ==, pid_list_length);
  for (size_t i = 0; i < pid_list_length; ++i) {
    g_assert_cmpint(g_list_index(keys, GINT_TO_POINTER(process_ids[i])), !=,
                    -1);
  }

  g_list_free(keys);
  g_hash_table_destroy(parsed_output);
}

int main(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);

  // Parser tests
  g_test_add_func("/parser/test_reads_all_fields_in_a_record",
                  test_reads_all_fields_in_a_record);

  // Main tests
  g_test_add_func("/callasof/test_fails_if_lsof_cannot_be_found",
                  test_fails_if_lsof_cannot_be_found);
  g_test_add_func("/callasof/test_lsof_path_initialized_to_default",
                  test_lsof_path_initialized_to_default);
  g_test_add_func("/callasof/test_provides_pid_map", test_provides_pid_map);

  return g_test_run();
}
