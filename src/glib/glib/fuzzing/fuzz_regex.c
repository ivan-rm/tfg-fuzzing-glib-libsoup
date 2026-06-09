#include "fuzz.h"

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  // 4 bytes for flags + 1 byte for pattern + 1 byte for search string
  if (size < 6)
    return 0;

  fuzz_set_logging_func ();

  uint32_t flags_raw;
  memcpy (&flags_raw, data, sizeof (uint32_t));

  GRegexCompileFlags compile_flags = (GRegexCompileFlags) (flags_raw & 0x1FFFF);
  compile_flags |= G_REGEX_RAW; // Force raw mode
  GRegexMatchFlags match_flags = (GRegexMatchFlags) ((flags_raw >> 16) & 0x1FFFF);

  const unsigned char *payload = data + 4;
  size_t payload_size = size - 4;

  size_t pattern_len = payload_size / 2;
  size_t search_len = payload_size - pattern_len;

  gchar *pattern = g_strndup ((const gchar *) payload, pattern_len);
  gchar *string = g_strndup ((const gchar *) (payload + pattern_len), search_len);

  GError *error = NULL;
  GRegex *regex = g_regex_new (pattern, compile_flags, match_flags, &error);

  if (regex)
    {
      GMatchInfo *match_info = NULL;
      int start_pos = (search_len > 0) ? (data[size - 1] % search_len) : 0;

      g_regex_match_full (regex, string, (gssize) search_len, start_pos, match_flags, &match_info, NULL);

      if (match_info)
        {
          g_match_info_free (match_info);
        }
      g_regex_unref (regex);
    }
  else
    {
      g_clear_error (&error);
    }

  g_free (pattern);
  g_free (string);

  return 0;
}
