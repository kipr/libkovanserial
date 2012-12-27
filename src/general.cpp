#include "kovanserial/general.hpp"

#include <sys/time.h>

long msystime()
{
	timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1000L + t.tv_usec / 1000L;
}