#include "callasof/callasof.h"

#include "parser_test.h"

#include "glib.h"

void test_reads_all_fields_in_a_record() {
  const gchar test_input[] = "p9097\0g9097\0R23617\0czsh\0u1000\0Llogout\0\n";
  const gsize test_input_length =
      sizeof(test_input) - 1; // subtract trailing 0 byte
  GByteArray *input_array = g_byte_array_sized_new(test_input_length);
  g_byte_array_append(input_array, (const guint8 *)test_input,
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