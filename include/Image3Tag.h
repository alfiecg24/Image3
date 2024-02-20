#ifndef IMAGE_3_TAG_H
#define IMAGE_3_TAG_H

#include <stdint.h>
#include <stdbool.h>

struct Image3Tag {
    uint32_t	tag;
	uint32_t	skipDistance;
	uint32_t	bufferLength;
	uint8_t	   *buffer;
};

#define IMAGE_3_TAG_IS_TAG(tag, tagType) strcmp(image3_type_to_string(tag->tag), tagType) == 0

bool image3_init_tags(struct Image3 *image);
void image3_save_important_tags(struct Image3 *image);
void image3_print_tags(struct Image3 *image);

#endif // IMAGE_3_TAG_H