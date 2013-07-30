/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 * 
 * Modifications by Braden McDorman for C++
 */

#ifndef _SHA1_HPP_
#define _SHA1_HPP_

#include <stdint.h>
#include "export.h"

class DLL_EXPORT SHA1
{
public:
	SHA1();
	
	void update(const uint8_t *data, uint32_t len);
	void final(uint8_t digest[20]);
	
private:
	void transform(uint32_t state[5], const uint8_t buffer[64]);
	
	uint32_t state[5];
	uint32_t count[2];
	uint8_t buffer[64];
};


#endif /* _SHA1_HPP_ */