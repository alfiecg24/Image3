#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

int memcmp_masked(const void *str1, const void *str2, unsigned char* mask, size_t n);
void enumerate_range(uint64_t start, uint64_t end, uint16_t alignment, size_t nbytes, bool (^enumerator)(uint64_t cur));
char *image3_type_to_string(uint32_t type);

#endif