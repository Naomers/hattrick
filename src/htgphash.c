//general purpose hashing code.
#include "htgphash.h"

uint32_t fnv1a_32(const char *bytes, size_t len){
	uint32_t h = FNV_OFFSET_BASIS_32;
	for(size_t i = 0; i < len; i++){
		h ^= bytes[i];
		h *= FNV_PRIME_32;
	}
	return h;
}
