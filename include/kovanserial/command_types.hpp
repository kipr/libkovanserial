#ifndef _COMMAND_TYPES_HPP_
#define _COMMAND_TYPES_HPP_

#include <stdint.h>

#define COMMAND_ACTION_RUN ("run")
#define COMMAND_ACTION_COMPILE ("compile")
#define COMMAND_ACTION_READ ("read")
#define COMMAND_ACTION_SCREENSHOT ("scsh")

#define KOVAN_PROPERTY_DISPLAY_NAME ("dsnm")
#define KOVAN_PROPERTY_VERSION ("version")
#define KOVAN_PROPERTY_DEVICE ("device")
#define KOVAN_PROPERTY_SERIAL ("serial")

#define AUTH_TYPE_AUTHED 1
#define AUTH_TYPE_NEED_SECRET 2
#define AUTH_TYPE_GO_AWAY 3

namespace Command
{
	enum Type
	{
		None = 0,
		KnockKnock,
		WhosThere,
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
		char metadata[200];
		char dest[256];
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
