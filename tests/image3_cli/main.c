
#include <Image3.h>
#include <Image3Tag.h>
#include <stdio.h>
#include <stdbool.h>
#include <CommandLine.h>
#include <MemoryStream.h>
#include <FileStream.h>
#include <Util.h>
#include <Image3Keybag.h>
#include <Image3Cert.h>

void printUsage(char *argv0) {
    printf("Arguments:\n");
    printf("\t-i: input file\n");
    printf("\t-h: print information about the header\n");
    printf("\t-t: print information about the tags\n\n");
    printf("%s -i <input file>\n", argv0);
}

int main(int argc, char *argv[]) {
    char *inputFile = get_argument_value(argc, argv, "-i");
    bool printHeaderInfo = argument_exists(argc, argv, "-h");
    bool printTagInfo = argument_exists(argc, argv, "-t");
    if (!inputFile) {
        printUsage(argv[0]);
        return 1;
    }

    MemoryStream *stream = file_stream_init_from_path(inputFile, 0, FILE_STREAM_SIZE_AUTO, 0);
    if (!stream) {
        printf("Error: could not initialise memory stream!\n");
        return 1;
    }

    struct Image3 *image = image3_init(stream);
    if (!image) {
        printf("Error: could not initialise image3!\n");
        return 1;
    }

    if (printHeaderInfo) {
        printf("Image header:\n");
        printf("\tMagic: %s\n", image3_type_to_string(image->header->magic));
        printf("\tSkip distance: 0x%x\n", image->header->skipDistance);
        printf("\tBuffer length: 0x%x\n", image->header->bufferLength);
        printf("\tSigned length: 0x%x\n", image->header->signedLength);
        printf("\tType: %s\n", image3_type_to_string(image->header->type));
        printf("\n");
    }

    image3_save_important_tags(image);

    if (printTagInfo) {
        image3_print_tags(image);
    }

    // Iterate over nested images
    if (image->nestedImageCount > 0) {
        for (int i = 0; i < image->nestedImageCount; i++) {
            struct Image3 *nestedImage = image->nestedImages[i];
            if (printHeaderInfo) {
                printf("Nested image header:\n");
                printf("\tMagic: %s\n", image3_type_to_string(nestedImage->header->magic));
                printf("\tSkip distance: 0x%x\n", nestedImage->header->skipDistance);
                printf("\tBuffer length: 0x%x\n", nestedImage->header->bufferLength);
                printf("\tSigned length: 0x%x\n", nestedImage->header->signedLength);
                printf("\tType: %s\n", image3_type_to_string(nestedImage->header->type));
                printf("\n");
            }

            image3_save_important_tags(nestedImage);

            if (printTagInfo) {
                image3_print_tags(nestedImage);
            }
        }
    }

    image3_free(image);
    
    return 0;
}