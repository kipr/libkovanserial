#ifndef _RANDOMIZE_HPP_
#define _RANDOMIZE_HPP_

#include <stdint.h>

namespace randomize
{
	void seed();
	void seedOnce();
	void fill(uint8_t *const data, const uint64_t length);
}

#endif
