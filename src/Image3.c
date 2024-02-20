#include <MemoryStream.h>
#include <Image3.h>
#include <Image3Tag.h>
#include <stdio.h>

struct Image3 *image3_init(MemoryStream *stream)
{
    struct Image3 *image = malloc(sizeof(struct Image3));
    if (!image) return NULL;
    image->stream = stream;
    image->header = image3_header_init_from_memory_stream(stream);
    if (!image->header) {
        free(image);
        return NULL;
    }

    if (!image3_init_tags(image)) {
        free(image->header);
        free(image);
        return NULL;
    }

    return image;
}

void image3_free(struct Image3 *image)
{
    if (image->header) {
        free(image->header);
    }
    if (image->tags) {
        for (int i = 0; i < image->tagCount; i++) {
            free(image->tags[i]);
        }
        free(image->tags);
    }
    if (image->keybags) {
        for (int i = 0; i < image->keybagCount; i++) {
            free(image->keybags[i]);
        }
        free(image->keybags);
    }

    if (image->nestedImages) {
        for (int i = 0; i < image->nestedImageCount; i++) {
            image3_free(image->nestedImages[i]);
        }
        free(image->nestedImages);
    }

    memory_stream_free(image->stream);
    free(image);
}