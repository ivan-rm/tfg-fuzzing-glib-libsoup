#include "fuzz.h"

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  if (size < 40)
    return 0;

  fuzz_set_logging_func ();

  GScanner *scanner = g_scanner_new (NULL);
  if (!scanner)
    return 0;

  // Map bits to config
  uint32_t b = *((uint32_t *) data);
  scanner->config->case_sensitive = (b >> 0) & 1;
  scanner->config->skip_comment_multi = (b >> 1) & 1;
  scanner->config->skip_comment_single = (b >> 2) & 1;
  scanner->config->scan_comment_multi = (b >> 3) & 1;
  scanner->config->scan_identifier = (b >> 4) & 1;
  scanner->config->scan_identifier_1char = (b >> 5) & 1;
  scanner->config->scan_symbols = (b >> 6) & 1;
  scanner->config->scan_binary = (b >> 7) & 1;
  scanner->config->scan_octal = (b >> 8) & 1;
  scanner->config->scan_float = (b >> 9) & 1;
  scanner->config->scan_hex = (b >> 10) & 1;
  scanner->config->scan_hex_dollar = (b >> 11) & 1;
  scanner->config->scan_string_sq = (b >> 12) & 1;
  scanner->config->scan_string_dq = (b >> 13) & 1;
  scanner->config->identifier_2_string = (b >> 14) & 1;
  scanner->config->symbol_2_token = (b >> 15) & 1;
  scanner->config->scope_0_fallback = (b >> 16) & 1;

  // Symbols to test search logic
  char sym1[4], sym2[4];
  memcpy (sym1, data + 4, 3);
  sym1[3] = '\0';
  memcpy (sym2, data + 7, 3);
  sym2[3] = '\0';
  g_scanner_scope_add_symbol (scanner, 0, sym1, (void *) "t1");
  g_scanner_scope_add_symbol (scanner, 1, sym2, (void *) "t2");

  // Dynamic input name for error reporting paths
  scanner->input_name = (b >> 17) & 1 ? "fuzz_input" : NULL;

  // Handle identifier character set
  if (data[10] % 2 == 0)
    {
      scanner->config->cset_identifier_first = G_CSET_a_2_z "_";
    }

  g_scanner_input_text (scanner, (const gchar *) (data + 20), (guint) (size - 20));

  while (!g_scanner_eof (scanner))
    {
      if (data[11] % 5 == 0)
        {
          g_scanner_peek_next_token (scanner);
        }

      GTokenType token = g_scanner_get_next_token (scanner);
      if (token == G_TOKEN_EOF)
        break;

      // Lookup symbol
      if (token == G_TOKEN_IDENTIFIER && scanner->value.v_identifier)
        {
          g_scanner_lookup_symbol (scanner, scanner->value.v_identifier);
        }

      // Access values
      if (token == G_TOKEN_SYMBOL)
        {
          volatile gpointer p = scanner->value.v_symbol;
          (void) p;
        }
      else if (token == G_TOKEN_STRING || token == G_TOKEN_IDENTIFIER)
        {
          if (scanner->value.v_string)
            {
              volatile char c = scanner->value.v_string[0];
              (void) c;
            }
        }

      // Randomly switch scopes
      if (data[12] % 10 == 0)
        {
          g_scanner_set_scope (scanner, (data[12] % 2));
        }

      // Trigger error paths
      if (data[13] % 50 == 0)
        {
          g_scanner_unexp_token (scanner, G_TOKEN_INT, NULL, NULL, NULL, "fuzz", 1);
        }
      // Randomly remove symbols
      if (data[14] % 10 == 0)
        {
          g_scanner_scope_remove_symbol (scanner, 0, sym1);
        }
    }

  g_scanner_destroy (scanner);
  return 0;
}
