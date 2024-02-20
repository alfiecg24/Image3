
#include <Image3.h>
#include <Image3Tag.h>
#include <Image3Cert.h>
#include <MemoryStream.h>
#include <BufferedStream.h>
#include <Util.h>

#include <libDER/libDER.h>
#include <libDER/asn1Types.h>
#include <libDER/DER_CertCrl.h>
#include <libDER/DER_Keys.h>
#include <stdio.h>

bool der_byte_compare(DERByte *a, DERByte *b, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

int parse_tbs_cert(unsigned char *certData, unsigned certDataLen, bool *hasNestedImage, void **nestedImage, size_t *nestedImageSize)
{
	DERSignedCertCrl signedCert;
	DERTBSCert tbs;
	DERReturn drtn;
	DERItem item;
	int verbose = 1;

	/* Top level decode of signed cert into 3 components */
	item.data = certData;
	item.length = certDataLen;
	drtn = DERParseSequence(&item, DERNumSignedCertCrlItemSpecs, DERSignedCertCrlItemSpecs,
		&signedCert, sizeof(signedCert));
	if(drtn) {
		printf("DERParseSequence(SignedCert) - %d\n", drtn);
		return -1;
    }
	
	
	/* decode the TBSCert - it was saved in full DER form */
	drtn = DERParseSequence(&signedCert.tbs, 
		DERNumTBSCertItemSpecs, DERTBSCertItemSpecs,
		&tbs, sizeof(tbs));
	if(drtn) {
		printf("DERParseSequenceContent(TBSCert) - %d\n", drtn);
		return -1;
	}

    if (tbs.extensions.data) {
        DERItem extensions = tbs.extensions;
        DERSequence sequence;
        DERTag tag;
        drtn = DERDecodeSeqInit(&extensions, &tag, &sequence);
        if (drtn) {
            printf("DERDecodeSeqInit(extensions) - %d\n", drtn);
            return -1;
        }

        if (tag != ASN1_CONSTR_SEQUENCE) {
            printf("Invalid extensions tag\n");
            return -1;
        }

        while (sequence.nextItem != sequence.end) {
            DERExtension extension;
            DERDecodedInfo decodedInfo;

            drtn = DERDecodeSeqNext(&sequence, &decodedInfo);
            if (drtn) {
                printf("DERDecodeSeqNext(extensions) - %d\n", drtn);
                return -1;
            }

            drtn = DERParseSequenceContent(&decodedInfo.content, DERNumExtensionItemSpecs, DERExtensionItemSpecs, &extension, sizeof(extension));
            if (drtn) {
                printf("DERParseSequenceContent(extension) - %d\n", drtn);
                return -1;
            }

            DERByte nestedImageOid[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x63, 0x64, 0x06, 0x01, 0x01 };
            if (der_byte_compare(extension.extnID.data, nestedImageOid, extension.extnID.length)) {
                if (hasNestedImage) {
                    *hasNestedImage = true;
                    DERItem nestedImageItem = extension.extnValue;
                    DERDecodedInfo nestedImageDecodedInfo;
                    drtn = DERDecodeItem(&nestedImageItem, &nestedImageDecodedInfo);
                    if (drtn) { 
                        printf("DERDecodeItem(nestedImage) - %d\n", drtn);
                        return -1;
                    }
                    if (nestedImage) {
                        *nestedImage = nestedImageItem.data + 3;
                    }
                    if (nestedImageSize) {
                        *nestedImageSize = nestedImageItem.length - 3;
                    }
                }
            }
        
        }
    }
	
	return 0;
}

bool image3_add_nested_images(struct Image3 *image, struct Image3Tag *tag) {
    bool hasNestedImage;
    void *nestedImage;
    size_t nestedImageSize;

    // Make sure the first two bytes are 0x30 0x82
    if(*(uint8_t *)tag->buffer != 0x30 || *((uint8_t *)tag->buffer + 1) != 0x82) {
        printf("Invalid CERT buffer - this is not a DER sequence!\n");
        return false;
    }

    uint16_t firstCertLength = (*(uint8_t *)(tag->buffer + 2) << 8 | *(uint8_t *)(tag->buffer + 3)) + 4;
    if (firstCertLength > tag->bufferLength) {
        printf("Error: first cert length is larger than tag buffer length\n");
        return false;
    }

    uint16_t secondCertLength = (*(uint8_t *)(tag->buffer + firstCertLength + 2) << 8 | *(uint8_t *)(tag->buffer + firstCertLength + 3)) + 4;
    if (secondCertLength > tag->bufferLength - firstCertLength) {
        printf("Error: second cert length is larger than tag buffer length\n");
        return false;
    }

    // First, we get the total size of all the nested images
    if (parse_tbs_cert(tag->buffer, firstCertLength, &hasNestedImage, &nestedImage, &nestedImageSize) != 0) {
        printf("Error: failed to parse TBS cert\n");
        return false;
    }

    if (hasNestedImage) {
        image->nestedImageCount++;
    }

    hasNestedImage = false;
    nestedImageSize = 0;
    nestedImage = NULL;

    if (parse_tbs_cert(tag->buffer + firstCertLength, secondCertLength, &hasNestedImage, &nestedImage, &nestedImageSize) != 0) {
        printf("Error: failed to parse TBS cert\n");
        return false;
    }

    if (hasNestedImage) {
        image->nestedImageCount++;
    }

    hasNestedImage = false;

    // Then, we allocate space for the nested images and copy them over

    if (image->nestedImageCount == 0) {
        return true;
    }

    image->nestedImages = malloc(image->nestedImageCount * sizeof(struct Image3 *));
    image->nestedImageCount = 0;

    if (parse_tbs_cert(tag->buffer, firstCertLength, &hasNestedImage, &nestedImage, &nestedImageSize) != 0) {
        printf("Error: failed to parse TBS cert\n");
        return false;
    }

    if (hasNestedImage) {
        MemoryStream *stream = buffered_stream_init_from_buffer(nestedImage, nestedImageSize, 0);
        struct Image3 *nestedImage3 = image3_init(stream);
        if (!nestedImage3) {
            printf("Error: failed to initialise nested image\n");
            return false;
        }

        image->nestedImages[image->nestedImageCount] = nestedImage3;
        image->nestedImageCount++;
    }

    hasNestedImage = false;

    if (parse_tbs_cert(tag->buffer + firstCertLength, secondCertLength, &hasNestedImage, &nestedImage, &nestedImageSize) != 0) {
        printf("Error: failed to parse TBS cert\n");
        return false;
    }

    if (hasNestedImage) {
        MemoryStream *stream = buffered_stream_init_from_buffer(nestedImage, nestedImageSize, 0);
        struct Image3 *nestedImage3 = image3_init(stream);
        if (!nestedImage3) {
            printf("Error: failed to initialise nested image\n");
            return false;
        }
        
        image->nestedImages[image->nestedImageCount] = nestedImage3;
        image->nestedImageCount++;
    }

    return true;
}