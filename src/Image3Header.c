#include <Image3ByteOrder.h>
#include <Image3Header.h>
#include <MemoryStream.h>
#include <Util.h>

struct Image3Header *image3_header_init_from_memory_stream(MemoryStream *stream)
{
    struct Image3Header *header = malloc(sizeof(struct Image3Header));
    if (!header) return NULL;

    int r = memory_stream_read(stream, 0, sizeof(struct Image3Header), header);
    if (r != 0) {
        free(header);
        return NULL;
    }

    IMAGE_3_HEADER_APPLY_BYTE_ORDER(header, LITTLE_TO_HOST_APPLIER);

    return header;
}