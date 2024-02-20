#include <Image3.h>
#include <Image3Tag.h>
#include <Image3Keybag.h>
#include <stdio.h>
#include <stdlib.h>

char *kbag_selector_to_string(uint32_t selector) {
    switch(selector) {
        case kImage3KeybagSelectorNoKey:
            return "No Key";
        case kImage3KeybagSelectorChipUnique:
            return "Chip Unique (Prod)";
        case kImage3KeybagSelectorChipUniqueDev:
            return "Chip Unique (Dev)";
        default:
            return "Unknown";
    }
}

bool image3_add_keybag(struct Image3 *image, struct Image3Tag *tag) {
    struct Image3Keybag *keybag = (struct Image3Keybag *)tag->buffer;
    if (tag->bufferLength != sizeof(struct Image3Keybag)) {
        printf("Error: keybag tag has invalid length\n");
        return false;
    }

    if (image->keybagCount >= 2) {
        printf("Warning: image already has %d keybags, found another\n", image->keybagCount);
    }

    if (image->keybagCount == 0) {
        image->keybags = malloc(sizeof(struct Image3Keybag));
    } else {
        image->keybags = realloc(image->keybags, sizeof(struct Image3Keybag) * (image->keybagCount + 1));
    }
    image->keybags[image->keybagCount] = keybag;
    image->keybagCount++;
    
    return true;
}
