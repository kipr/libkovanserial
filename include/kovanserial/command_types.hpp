#ifndef _COMMAND_TYPES_HPP_
#define _COMMAND_TYPES_HPP_

#include <stdint.h>

namespace Command
{
	enum Type
	{
		None = 0,
		Hangup,
		FileAction,
		FileActionConfirm,
		FileActionProgress,
		ListProperties,
		Property,
		FileHeader,
		FileConfirm,
		File
	};
	
	struct PropertyData
	{
		char name[8];
	};
	
	struct PropertyValuePairData
	{
		char name[8];
		char value[128];
	};
	
	struct PropertyListData
	{
		size_t size;
		PropertyData names[50];
	};
	
	struct FileHeaderData
	{
		uint32_t size;
		char dest[500];
	};
	
	struct FileActionData
	{
		char action[8];
		char dest[500];
	};
	
	struct FileActionProgressData
	{
		bool finished;
		double progress;
	};
}

#endif
