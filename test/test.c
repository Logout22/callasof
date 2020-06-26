#include "callasof/callasof.h"

#include "test_output.h"

#include <stdio.h>
#include <string.h>

// Parser tests
GPtrArray *test_reads_all_fields_in_a_record_process_records = NULL;
GHashTable *test_reads_all_fields_in_a_record_result = NULL;

static void
test_reads_all_fields_in_a_record_on_record_parsed(GPtrArray *process_records,
                                                   GHashTable *record) {
  g_ptr_array_ref(process_records);
  test_reads_all_fields_in_a_record_process_records = process_records;
  g_hash_table_ref(record);
  test_reads_all_fields_in_a_record_result = record;
}

static void test_reads_all_fields_in_a_record() {
  const gsize test_input_length =
      STATIC_BYTE_ARRAY_LENGTH(example_single_line_output);
  GByteArray *input_array = g_byte_array_sized_new(test_input_length);
  g_byte_array_append(input_array, (const guint8 *)example_single_line_output,
                      test_input_length);

  ParserCallbacks callbacks = {
      .on_record_parsed = test_reads_all_fields_in_a_record_on_record_parsed};

  g_assert(parse_lsof_output(input_array, &callbacks));

  const guint element_count = 6;
  g_assert_cmpuint(element_count, ==,
                   g_hash_table_size(test_reads_all_fields_in_a_record_result));
  gchar *expected_output[][2] = {{"p", "9097"}, {"g", "9097"}, {"R", "23617"},
                                 {"c", "zsh"},  {"u", "1000"}, {"L", "logout"}};
  for (guint i = 0; i < element_count; ++i) {
    gint key = expected_output[i][0][0];
    g_assert_cmpstr(
        expected_output[i][1], ==,
        (gchar *)g_hash_table_lookup(test_reads_all_fields_in_a_record_result,
                                     GINT_TO_POINTER(key)));
  }

  g_hash_table_unref(test_reads_all_fields_in_a_record_result);
  g_ptr_array_unref(test_reads_all_fields_in_a_record_process_records);
  g_byte_array_free(input_array, TRUE);
}

GPtrArray *test_reads_all_records_for_a_process_result = NULL;

static void test_reads_all_records_for_a_process_process_parsed(
    GPtrArray *process_records) {
  g_ptr_array_ref(process_records);
  test_reads_all_records_for_a_process_result = process_records;
}

static void test_reads_all_records_for_a_process() {
  const gsize test_input_length =
      STATIC_BYTE_ARRAY_LENGTH(example_single_process_output);
  GByteArray *input_array = g_byte_array_sized_new(test_input_length);
  g_byte_array_append(input_array,
                      (const guint8 *)example_single_process_output,
                      test_input_length);

  ParserCallbacks callbacks = {
      .on_process_parsed = test_reads_all_records_for_a_process_process_parsed};

  g_assert(parse_lsof_output(input_array, &callbacks));

  const guint element_count = 4;
  g_assert_cmpuint(element_count, ==,
                   test_reads_all_records_for_a_process_result->len);
  gchar *expected_first_record[][2] = {{"p", "9097"},  {"g", "9097"},
                                       {"R", "23617"}, {"c", "zsh"},
                                       {"u", "1000"},  {"L", "logout"}};
#define test_reads_all_records_for_a_process_file_field_count 9
  gchar *expected_file_records
      [][test_reads_all_records_for_a_process_file_field_count][2] = {
          {{"f", "cwd"},
           {"a", " "},
           {"l", " "},
           {"t", "DIR"},
           {"D", "0xfd02"},
           {"s", "4096"},
           {"i", "24251829"},
           {"k", "7"},
           {"n", "/home/logout/devtrees/callasof"}},
          {{"f", "rtd"},
           {"a", " "},
           {"l", " "},
           {"t", "DIR"},
           {"D", "0xfd01"},
           {"s", "4096"},
           {"i", "2"},
           {"k", "31"},
           {"n", "/"}},
          {{"f", "txt"},
           {"a", " "},
           {"l", " "},
           {"t", "REG"},
           {"D", "0xfd01"},
           {"s", "832416"},
           {"i", "2097559"},
           {"k", "1"},
           {"n", "/bin/zsh"}},
      };

  const guint pid_field_count = 6;
  GHashTable *first_record =
      g_ptr_array_index(test_reads_all_records_for_a_process_result, 0);
  for (guint i = 0; i < pid_field_count; ++i) {
    gint key = expected_first_record[i][0][0];
    g_assert_cmpstr(
        expected_first_record[i][1], ==,
        (gchar *)g_hash_table_lookup(first_record, GINT_TO_POINTER(key)));
  }
  for (guint i = 1; i < element_count; i++) {
    GHashTable *current_record =
        g_ptr_array_index(test_reads_all_records_for_a_process_result, i);
    for (guint j = 0; j < test_reads_all_records_for_a_process_file_field_count;
         j++) {
      gint key = expected_file_records[i - 1][j][0][0];
      g_assert_cmpstr(
          expected_file_records[i - 1][j][1], ==,
          (gchar *)g_hash_table_lookup(current_record, GINT_TO_POINTER(key)));
    }
  }

  g_ptr_array_unref(test_reads_all_records_for_a_process_result);
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

  g_assert(parse_lsof_output(input_array, NULL));
  /* TODO re-capture output:
   *
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
  g_hash_table_destroy(parsed_output);*/
}

int main(int argc, char *argv[]) {
  g_test_init(&argc, &argv, NULL);

  // Parser tests
  g_test_add_func("/parser/test_reads_all_fields_in_a_record",
                  test_reads_all_fields_in_a_record);
  g_test_add_func("/parser/test_reads_all_records_for_a_process",
                  test_reads_all_records_for_a_process);

  // Main tests
  g_test_add_func("/callasof/test_fails_if_lsof_cannot_be_found",
                  test_fails_if_lsof_cannot_be_found);
  g_test_add_func("/callasof/test_lsof_path_initialized_to_default",
                  test_lsof_path_initialized_to_default);
  g_test_add_func("/callasof/test_provides_pid_map", test_provides_pid_map);

  return g_test_run();
}
