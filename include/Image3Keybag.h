#ifndef IMAGE_3_KEYBAG_H
#define IMAGE_3_KEYBAG_H

#include <stdint.h>
#include <stdbool.h>

struct Image3Keybag {
    uint32_t kbSelector;
    uint32_t kbKeySize;
    uint8_t kbIVBytes[16];
    uint8_t kbKeyBytes[32];
};

#define kImage3KeybagSelectorNoKey              (0)
#define kImage3KeybagSelectorChipUnique         (1)
#define kImage3KeybagSelectorChipUniqueDev      (2)

char *kbag_selector_to_string(uint32_t selector);
bool image3_add_keybag(struct Image3 *image, struct Image3Tag *tag);

#endif // IMAGE_3_KEYBAG_H