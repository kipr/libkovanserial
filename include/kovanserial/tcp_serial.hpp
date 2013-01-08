#ifndef _TCP_SERIAL_HPP_
#define _TCP_SERIAL_HPP_

#include "tcp.hpp"

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#endif

class TcpSerial : public Tcp
{
public:
	TcpSerial(const char *host, const char *service);
	
	virtual bool makeAvailable();
	virtual void endSession();
	virtual bool isReliable() const;

private:
	char m_host[128];
	char m_service[128];
};

#endif
