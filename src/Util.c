#include <Util.h>
#include <stdio.h>

int memcmp_masked(const void *str1, const void *str2, unsigned char* mask, size_t n)
{
    const unsigned char* p = (const unsigned char*)str1;
    const unsigned char* q = (const unsigned char*)str2;

    if (p == q) return 0;
    for (int i = 0; i < n; i++) {
        unsigned char cMask = 0xFF;
        if (mask) {
            cMask = mask[i];
        }
        if((p[i] & cMask) != (q[i] & cMask)) {
            // we do not care about 1 / -1
            return -1;
        }
    }

    return 0;
}

void enumerate_range(uint64_t start, uint64_t end, uint16_t alignment, size_t nbytes, bool (^enumerator)(uint64_t))
{
    if (start == end) return;
    if (alignment == 0) return;
    if (nbytes == 0) return;
    if (nbytes % alignment) return;

    int dir = start < end ? 1 : -1;

    if (dir == 1) {
        end -= nbytes;
        if (start >= end) return;
    }
    else {
        start -= nbytes;
        if (start <= end) return;
    }

    for (uint64_t cur = start; (cur + (alignment * dir)) != end; cur += (dir * alignment)) {
        if (!enumerator(cur)) break;
    }
}

char *image3_type_to_string(uint32_t type) {
    char *typeStr = (char *)&type;
    char *str = malloc(5);
    if (!str) return NULL;
    str[0] = typeStr[3];
    str[1] = typeStr[2];
    str[2] = typeStr[1];
    str[3] = typeStr[0];
    str[4] = 0;
    return str;
}