#include "fuzz.h"
#include "soup-body-input-stream-http2.h"
#include <stdint.h>

int
LLVMFuzzerTestOneInput (const uint8_t *data, size_t size)
{
        if (size < 2)
                return 0;
        fuzz_set_logging_func ();

        GInputStream *stream = soup_body_input_stream_http2_new ();
        SoupBodyInputStreamHttp2 *bistream = SOUP_BODY_INPUT_STREAM_HTTP2 (stream);

        // Use the first byte to decide how to split the data into chunks
        uint8_t chunk_split = data[0] % (size / 2 + 1) + 1;
        const uint8_t *payload = data + 1;
        size_t payload_size = size - 1;

        for (size_t i = 0; i < payload_size; i += chunk_split) {
                size_t current_chunk = (i + chunk_split > payload_size) ? (payload_size - i) : chunk_split;

                soup_body_input_stream_http2_add_data (bistream, payload + i, current_chunk);

                // Periodically read back
                uint8_t buffer[1024];
                g_input_stream_read (stream, buffer, sizeof (buffer), NULL, NULL);
        }

        g_object_unref (stream);
        return 0;
}
