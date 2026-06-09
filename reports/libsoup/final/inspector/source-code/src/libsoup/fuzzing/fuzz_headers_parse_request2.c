#include "fuzz.h"

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
        if (size < 1)
                return 0;
        fuzz_set_logging_func ();

        SoupMessageHeaders *headers = soup_message_headers_new (SOUP_MESSAGE_HEADERS_REQUEST);
        char *method = NULL;
        char *path = NULL;
        SoupHTTPVersion ver;

        guint status;
        status = soup_headers_parse_request ((const char *)data, size, headers, &method, &path, &ver);

        if (status == SOUP_STATUS_OK) {

                GHashTable *ct_params = NULL;
                soup_message_headers_get_content_type (headers, &ct_params);
                if (ct_params)
                        g_hash_table_destroy (ct_params);

                char *disposition = NULL;
                GHashTable *cd_params = NULL;
                if (soup_message_headers_get_content_disposition (headers, &disposition, &cd_params)) {
                        g_free (disposition);
                        if (cd_params)
                                g_hash_table_destroy (cd_params);
                }

                soup_message_headers_get_content_length (headers);
                soup_message_headers_get_encoding (headers);
                soup_message_headers_get_expectations (headers);

                SoupRange *ranges = NULL;
                int n_ranges;
                if (soup_message_headers_get_ranges (headers, 100, &ranges, &n_ranges)) {
                        soup_message_headers_free_ranges (headers, ranges);
                }
        }

        soup_message_headers_unref (headers);
        g_free (method);
        g_free (path);

        return 0;
}
