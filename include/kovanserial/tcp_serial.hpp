#ifndef _TCP_SERIAL_HPP_
#define _TCP_SERIAL_HPP_

#include "tcp.hpp"
#include "compat.hpp"

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#endif

class DLL_EXPORT TcpSerial : public Tcp
{
public:
	TcpSerial(const char *host, const unsigned short port);
	
	virtual bool makeAvailable();
	virtual void endSession();
	virtual bool isReliable() const;

private:
	char _host[128];
  unsigned short _port;
};

#endif
