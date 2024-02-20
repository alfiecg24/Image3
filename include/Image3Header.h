#ifndef IMAGE_3_HEADER_H
#define IMAGE_3_HEADER_H

#include <stdint.h>
#include <MemoryStream.h>

// Image3 Header structure
struct Image3Header {
    uint32_t magic;
    uint32_t skipDistance;
    uint32_t bufferLength;
    uint32_t signedLength;
    uint32_t type;
    uint8_t buffer[];
};

// Image3 Magic
#define IMAGE3_MAGIC      'Img3'

// Image Types
// TODO: Add more types
#define IMAGE_TYPE_ILLB   'illb'
#define IMAGE_TYPE_IBSS   'ibss'

struct Image3Header *image3_header_init_from_memory_stream(MemoryStream *stream);

#endif // IMAGE_3_HEADER_H