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
#include <iostream>

TcpSerial::TcpSerial(const char *host, const char *service)
{
	strncpy(m_host, host, 128);
	strncpy(m_service, service, 128);
}

bool TcpSerial::makeAvailable()
{
	closeFd();
	setFd(socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));
	addrinfo hints;
	addrinfo *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(m_host, m_service, &hints, &res);
	bool ret = ::connect(fd(), res->ai_addr, res->ai_addrlen);
#ifdef WIN32
	std::cout << "makeAvail ret = " << ret << " " << WSAGetLastError() << std::endl;
#endif
	freeaddrinfo(res);
	return ret == 0;
}


void TcpSerial::endSession()
{
	closeFd();
}

bool TcpSerial::isReliable() const
{
	return true;
}