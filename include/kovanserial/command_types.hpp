#ifndef _COMMAND_TYPES_HPP_
#define _COMMAND_TYPES_HPP_

#include <stdint.h>
#include "kovan_serial_compat.hpp"

#define COMMAND_ACTION_RUN ("run")
#define COMMAND_ACTION_COMPILE ("compile")
#define COMMAND_ACTION_READ ("read")
#define COMMAND_ACTION_SCREENSHOT ("scsh")

#define KOVAN_PROPERTY_DISPLAY_NAME ("dsnm")
#define KOVAN_PROPERTY_VERSION ("version")
#define KOVAN_PROPERTY_DEVICE ("device")
#define KOVAN_PROPERTY_SERIAL ("serial")

#define KOVAN_SERIAL_SESSION_KEY_SIZE (64)

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
		File,
		RequestAuthenticationInfo,
		AuthenticationInfo,
		RequestAuthentication,
		ConfirmAuthentication,
		RequestProtocolVersion,
		ProtocolVersion
	};
	
	struct DLL_EXPORT PropertyData
	{
		char name[8];
	};
	
	struct DLL_EXPORT PropertyValuePairData
	{
		char name[8];
		char value[128];
	};
	
	struct DLL_EXPORT PropertyListData
	{
		size_t size;
		PropertyData names[50];
	};
	
	struct DLL_EXPORT FileHeaderData
	{
		uint32_t size;
		char metadata[200];
		char dest[256];
	};
	
	struct DLL_EXPORT FileActionData
	{
		char action[8];
		char dest[500];
	};
	
	struct DLL_EXPORT FileActionProgressData
	{
		bool finished;
		double progress;
	};
	
	struct DLL_EXPORT AuthenticationInfoData
	{
		bool authNecessary;
	};
	
	struct DLL_EXPORT RequestAuthenticationData
	{
		// MD5 hash
		uint8_t password[16];
	};
	
	struct DLL_EXPORT ConfirmAuthenticationData
	{
		bool success;
		uint8_t scrambledSessionKey[KOVAN_SERIAL_SESSION_KEY_SIZE];
	};
	
	struct DLL_EXPORT ProtocolVersionData
	{
		char version[65];
	};
}

#endif
