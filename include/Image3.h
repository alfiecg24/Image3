#ifndef IMAGE_3_H
#define IMAGE_3_H

// #include <Image3Keybag.h>
#include <Image3Header.h>
#include <MemoryStream.h>

struct Image3 {
    struct Image3Header *header;
    struct Image3Tag **tags;
    int tagCount;
    MemoryStream *stream;
    struct Image3 **nestedImages;
    int nestedImageCount;
    struct Image3Keybag **keybags;
    int keybagCount;
};

struct Image3 *image3_init(MemoryStream *stream);
void image3_free(struct Image3 *image);

#endif // IMAGE_3_H