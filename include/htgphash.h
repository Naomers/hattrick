#ifndef HTGPHASH_H
#define HTGPHASH_H
#include <stdint.h>
#include <stddef.h> //I always forget size_t is an included type 

#define FNV_OFFSET_BASIS_32 2166136261U
#define FNV_PRIME_32        16777619U

uint32_t fnv1a_32(const char *bytes, size_t len);
#define hash32 fnv1a_32



#endif
