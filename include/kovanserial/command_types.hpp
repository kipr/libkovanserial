#ifndef _COMMAND_TYPES_HPP_
#define _COMMAND_TYPES_HPP_

#include <stdint.h>

namespace Command
{
	enum Type
	{
		None = 0,
		FileHeader,
		FileConfirm,
		File
	};
	
	struct FileHeaderData
	{
		uint32_t size;
		char dest[500];
	};
}

#endif
