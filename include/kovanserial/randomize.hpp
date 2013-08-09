#ifndef _RANDOMIZE_HPP_
#define _RANDOMIZE_HPP_

#include <stdint.h>
#include "kovan_serial_compat.hpp"

namespace randomize
{
	DLL_EXPORT void seed();
	DLL_EXPORT void seedOnce();
	DLL_EXPORT void fill(uint8_t *const data, const uint64_t length);
}

#endif
