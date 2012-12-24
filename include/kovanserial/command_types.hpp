#ifndef _COMMAND_TYPES_HPP_
#define _COMMAND_TYPES_HPP_

#include <stdint.h>

namespace Command
{
	enum Type
	{
		None = 0,
		FileHeader,
		File,
		FileResult
	};
	
	struct FileHeaderData
	{
		uint32_t size;
		char dest[4096];
	};
}

#endif
