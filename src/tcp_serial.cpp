#include "kovanserial/tcp_serial.hpp"

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#include <unistd.h>
#include <string.h>

TcpSerial::TcpSerial(const char *host, const char *service)
{
	strncpy(m_host, host, 128);
	strncpy(m_service, service, 128);
}

bool TcpSerial::makeAvailable()
{
	closeFd();
	setFd(socket(PF_INET, SOCK_STREAM, 0));
	addrinfo hints;
	addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(m_host, m_service, &hints, &res);
	bool ret = ::connect(fd(), res->ai_addr, res->ai_addrlen) == 0;
	freeaddrinfo(res);
	return ret;
}


void TcpSerial::endSession()
{
	closeFd();
}

bool TcpSerial::isReliable() const
{
	return true;
}