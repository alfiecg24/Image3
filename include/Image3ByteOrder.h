#ifndef IMAGE_3_BYTE_ORDER_H
#define IMAGE_3_BYTE_ORDER_H

#include <stdio.h>
#include <stdlib.h>
#include <libkern/OSByteOrder.h>

// 8-bit integers needed for CodeDirectory
#define BIG_TO_HOST(n) _Generic((n), \
    int8_t: n, \
    uint8_t: n, \
    int16_t: OSSwapBigToHostInt16(n), \
    uint16_t: OSSwapBigToHostInt16(n), \
    int32_t: OSSwapBigToHostInt32(n), \
    uint32_t: OSSwapBigToHostInt32(n), \
    int64_t: OSSwapBigToHostInt64(n), \
    uint64_t: OSSwapBigToHostInt64(n) \
)

#define HOST_TO_BIG(n) _Generic((n), \
    int8_t: n, \
    uint8_t: n, \
    uint16_t: OSSwapHostToBigInt16(n), \
    int16_t: OSSwapHostToBigInt16(n), \
    int32_t: OSSwapHostToBigInt32(n), \
    uint32_t: OSSwapHostToBigInt32(n), \
    int64_t: OSSwapHostToBigInt64(n), \
    uint64_t: OSSwapHostToBigInt64(n) \
)

#define LITTLE_TO_HOST(n) _Generic((n), \
    int8_t: n, \
    uint8_t: n, \
    int16_t: OSSwapLittleToHostInt16(n), \
    uint16_t: OSSwapLittleToHostInt16(n), \
    int32_t: OSSwapLittleToHostInt32(n), \
    uint32_t: OSSwapLittleToHostInt32(n), \
    int64_t: OSSwapLittleToHostInt64(n), \
    uint64_t: OSSwapLittleToHostInt64(n) \
)

#define HOST_TO_LITTLE(n) _Generic((n), \
    int8_t: n, \
    uint8_t: n, \
    int16_t: OSSwapHostToLittleInt16(n), \
    uint16_t: OSSwapHostToLittleInt16(n), \
    int32_t: OSSwapHostToLittleInt32(n), \
    uint32_t: OSSwapHostToLittleInt32(n), \
    int64_t: OSSwapHostToLittleInt64(n), \
    uint64_t: OSSwapHostToLittleInt64(n) \
)

#define HOST_TO_LITTLE_APPLIER(instance, member) \
    (instance)->member = HOST_TO_LITTLE((instance)->member)

#define HOST_TO_BIG_APPLIER(instance, member) \
    (instance)->member = HOST_TO_BIG((instance)->member)

#define LITTLE_TO_HOST_APPLIER(instance, member) \
    (instance)->member = LITTLE_TO_HOST((instance)->member)

#define BIG_TO_HOST_APPLIER(instance, member) \
    (instance)->member = BIG_TO_HOST((instance)->member)

#define IMAGE_3_HEADER_APPLY_BYTE_ORDER(header, applier) \
    applier(header, skipDistance); \
    applier(header, bufferLength); \
    applier(header, signedLength); \
    applier(header, type);

#define IMAGE_3_TAG_HEADER_APPLY_BYTE_ORDER(header, applier) \
    applier(header, skipDistance); \
    applier(header, bufferLength);

#endif // IMAGE_3_BYTE_ORDER_H