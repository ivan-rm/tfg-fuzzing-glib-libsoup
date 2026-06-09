#include "fuzz.h"

static const GVariantType *
get_type_from_data (unsigned char selector)
{
  static const char *types[] = {
    "y", "b", "n", "q", "i", "u", "x", "t", "d", "s", "o", "g", "v",
    "ay", "as", "a(yi)", "a{sv}", "(asitu)", "(v(ii)v)", "a(ds)",
    "m(ii)", "m(s)", "a{sd}", "aa(ii)"
  };
  const char *type_str = types[selector % G_N_ELEMENTS (types)];
  return G_VARIANT_TYPE (type_str);
}

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  if (size < 2)
    return 0;

  fuzz_set_logging_func ();

  const GVariantType *type = get_type_from_data (data[0]);

  GBytes *bytes = g_bytes_new (data + 1, size - 1);
  GVariant *variant = g_variant_new_from_bytes (type, bytes, FALSE);
  g_bytes_unref (bytes);

  if (!variant)
    return 0;

  GVariant *normal = g_variant_get_normal_form (variant);
  g_variant_unref (variant);

  GVariant *swapped = g_variant_byteswap (normal);
  if (swapped)
    {
      GVariant *unswapped = g_variant_byteswap (swapped);
      if (!g_variant_equal (normal, unswapped))
        {
          g_error ("¡ERROR!"); // Abort
        }
      g_variant_unref (unswapped);
      g_variant_unref (swapped);
    }

  g_variant_unref (normal);
  return 0;
}
