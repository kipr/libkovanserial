#include "kovanserial/xor.hpp"

bool xor_crypt::crypt(const uint8_t *const data, uint8_t *const output, const uint64_t size,
	const uint8_t *const key, const uint64_t keySize)
{
	if(!key || !data || !output || keySize < 1) return false;
	
	uint64_t pos = 0;
	for(uint64_t i = 0; i < size; ++i) {
		output[i] = data[i] ^ key[pos];
		
		++pos;
		pos %= keySize;
	}
	
	return true;
}