#include <Image3.h>
#include <Image3Keybag.h>
#include <MemoryStream.h>
#include <stddef.h>
#include <stdio.h>
#include <Image3Tag.h>
#include <Image3ByteOrder.h>
#include <Util.h>
#include <Image3Cert.h>

void image3_print_type_tag(struct Image3Tag *tag) {
    uint32_t type = *((uint32_t *)tag->buffer);
    printf("Type: %s\n", image3_type_to_string(type));
}

void image3_print_data_tag(struct Image3Tag *tag) {
    printf("Data length: 0x%x\n", tag->bufferLength);
}

void image3_print_bord_tag(struct Image3Tag *tag) {
    uint32_t bord = *((uint32_t *)tag->buffer);
    printf("Board ID: 0x%x\n", bord);
}

void image3_print_kbag_tag(struct Image3Tag *tag) {
    struct Image3Keybag *keybag = (struct Image3Keybag *)tag->buffer;
    printf("Keybag:\n");
    printf("\tType: %s\n", kbag_selector_to_string(keybag->kbSelector));
    printf("\tKey: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", keybag->kbKeyBytes[i]);
    }
    printf("\n");
    printf("\tIV: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", keybag->kbIVBytes[i]);
    }
    printf("\n");
}

void image3_print_ecid_tag(struct Image3Tag *tag) {
    uint64_t ecid = *((uint64_t *)tag->buffer);
    printf("ECID: 0x%.16llx\n", ecid);
}

void image3_print_chip_tag(struct Image3Tag *tag) {
    uint32_t chip = *((uint32_t *)tag->buffer);
    printf("Chip ID: 0x%x\n", chip);
}

void image3_print_sepo_tag(struct Image3Tag *tag) {
    uint32_t sepo = *((uint32_t *)tag->buffer);
    printf("Security epoch: 0x%x\n", sepo);
}

void image3_print_cepo_tag(struct Image3Tag *tag) {
    uint32_t cepo = *((uint32_t *)tag->buffer);
    printf("Chip epoch: 0x%x\n", cepo);
}

void image3_print_sdom_tag(struct Image3Tag *tag) {
    uint32_t sdom = *((uint32_t *)tag->buffer);
    printf("Security domain: 0x%x\n", sdom);
}

void image3_print_prod_tag(struct Image3Tag *tag) {
    uint32_t prod = *((uint32_t *)tag->buffer);
    printf("Production mode: 0x%x\n", prod);
}

void image_print_vers_tag(struct Image3Tag *tag) {
    uint32_t strlen = *((uint32_t *)tag->buffer);
    char *version = malloc(strlen + 1);
    if (!version) {
        printf("Error: could not allocate memory for version string\n");
        return;
    }

    memcpy(version, tag->buffer + 4, strlen);

    version[strlen] = 0;
    printf("Version: %s\n", version);
}

bool image3_init_tags(struct Image3 *image) {
    int position = offsetof(struct Image3Header, buffer);
    MemoryStream *buffer = memory_stream_softclone(image->stream);
    
    if (memory_stream_trim(buffer, position, 0) != 0) {
        printf("Error: could not trim buffer\n");
        memory_stream_free(buffer);
        return false;
    }

    int tagCount = 0;
    position = 0;
    while (position < image->header->bufferLength) {
        struct Image3Tag *tagHeader = malloc(sizeof(struct Image3Tag));
        if (!tagHeader) {
            printf("Error: could not allocate memory for tag header\n");
            memory_stream_free(buffer);
            return false;
        }

        if (memory_stream_read(buffer, position, sizeof(struct Image3Tag), tagHeader) != 0) {
            printf("Error: could not read tag header\n");
            free(tagHeader);
            memory_stream_free(buffer);
            return false;
        }

        IMAGE_3_TAG_HEADER_APPLY_BYTE_ORDER(tagHeader, LITTLE_TO_HOST_APPLIER);

        position += tagHeader->skipDistance;

        tagCount++;
        free(tagHeader);
    }

    image->tagCount = tagCount;

    // We iterate over all the tags again to get the actual tag data and allocate memory for the tags
    image->tags = malloc(sizeof(struct Image3Tag *) * tagCount);
    if (!image->tags) {
        printf("Error: could not allocate memory for tags\n");
        memory_stream_free(buffer);
        return false;
    }

    position = 0;
    for (int i = 0; i < tagCount; i++) {
        struct Image3Tag *tagHeader = malloc(sizeof(struct Image3Tag));
        if (!tagHeader) {
            printf("Error: could not allocate memory for tag header\n");
            memory_stream_free(buffer);
            return false;
        }

        if (memory_stream_read(buffer, position, sizeof(struct Image3Tag), tagHeader) != 0) {
            printf("Error: could not read tag header\n");
            free(tagHeader);
            memory_stream_free(buffer);
            return false;
        }

        IMAGE_3_TAG_HEADER_APPLY_BYTE_ORDER(tagHeader, LITTLE_TO_HOST_APPLIER);

        // Read data into tag buffer
        tagHeader->buffer = malloc(tagHeader->bufferLength);
        if (!tagHeader->buffer) {
            printf("Error: could not allocate memory for tag buffer\n");
            free(tagHeader);
            memory_stream_free(buffer);
            return false;
        }

        if (memory_stream_read(buffer, position + 12, tagHeader->bufferLength, tagHeader->buffer) != 0) {
            printf("Error: could not read tag buffer\n");
            free(tagHeader->buffer);
            free(tagHeader);
            memory_stream_free(buffer);
            return false;
        }

        image->tags[i] = tagHeader;
        position += tagHeader->skipDistance;
    }

    memory_stream_free(buffer);
    return true;
}

void image3_save_important_tags(struct Image3 *image) {
    for (int i = 0; i < image->tagCount; i++) {
        struct Image3Tag *tag = image->tags[i];
        if (IMAGE_3_TAG_IS_TAG(tag, "CERT")) {
            image3_add_nested_images(image, tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "KBAG")) {
            image3_add_keybag(image, tag);
        }
    }
}

void image3_print_tags(struct Image3 *image) {
    for (int i = 0; i < image->tagCount; i++) {
        struct Image3Tag *tag = image->tags[i];
        if (IMAGE_3_TAG_IS_TAG(tag, "TYPE")) {
            image3_print_type_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "DATA")) {
            image3_print_data_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "VERS")) {
            image_print_vers_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "BORD")) {
            image3_print_bord_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "KBAG")) {
            image3_print_kbag_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "ECID")) {
            image3_print_ecid_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "CHIP")) {
            image3_print_chip_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "SEPO")) {
            image3_print_sepo_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "CEPO")) {
            image3_print_cepo_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "SDOM")) {
            image3_print_sdom_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "PROD")) {
            image3_print_prod_tag(tag);
        } else if (IMAGE_3_TAG_IS_TAG(tag, "SHSH")) {
            printf("Image3 is signed and personalised!\n");
        } else if (IMAGE_3_TAG_IS_TAG(tag, "CERT")) {

        } else {
            printf("Tag: %s\n", image3_type_to_string(tag->tag));
        }
    }
    printf("\n");
}