#include "kovanserial/general.hpp"

#include <sys/time.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sched.h>
#endif


long msystime()
{
	timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec * 1000L + t.tv_usec / 1000L;
}

void yield()
{
#ifdef WIN32
	SleepEx(0, 0);
#else
	sched_yield();
#endif
}