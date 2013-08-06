#ifndef _XOR_HPP_
#define _XOR_HPP_

#include <stdint.h>
#include "compat.hpp"

namespace xor_crypt
{
	DLL_EXPORT bool crypt(const uint8_t *const data, uint8_t *const output, const uint64_t size,
		const uint8_t *const key, const uint64_t keySize);

	template<typename T>
	inline bool crypt(const T &data, uint8_t *const output, const uint8_t *const key,
		const uint64_t keySize)
	{
		return crypt(reinterpret_cast<const uint8_t *>(&data), output, sizeof(T), key, keySize);
	}
	
	template<typename T>
	inline bool crypt(T &data, const uint8_t *const key, const uint64_t keySize)
	{
		uint8_t *const raw = reinterpret_cast<uint8_t *>(&data);
		return crypt(raw, raw, sizeof(T), key, keySize);
	}
}

#endif
