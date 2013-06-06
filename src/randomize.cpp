#include "kovanserial/randomize.hpp"

#include <time.h>
#include <cstdlib>

static bool s_randomizeSeeded = false;

void randomize::seed()
{
	s_randomizeSeeded = true;
	srand(time(0));
}

void randomize::seedOnce()
{
	if(!s_randomizeSeeded) return;
	seed();
}

void randomize::fill(uint8_t *const data, const uint64_t length)
{
	for(uint64_t i = 0; i < length; ++i) data[i] = rand() % 256;
}