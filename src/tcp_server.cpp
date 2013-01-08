#include "kovanserial/tcp_server.hpp"

#include "kovanserial/general.hpp"
#include "socket_utils.hpp"

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
#include <errno.h>

TcpServer::TcpServer()
	: m_ourFd(-1)
{
	m_ourFd = socket(PF_INET, SOCK_STREAM, 0);
	setBlocking(m_ourFd, false);
}

TcpServer::~TcpServer()
{
	if(m_ourFd < 0) return;
	closeSocket(m_ourFd);

}

bool TcpServer::bind(const char *port)
{
	addrinfo hints;
	addrinfo *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if(getaddrinfo(NULL, port, &hints, &res) != 0) return false;
	
	bool ret = ::bind(m_ourFd, res->ai_addr, res->ai_addrlen) == 0;
	freeaddrinfo(res);
	return ret;
}

bool TcpServer::listen(const int &backLog)
{
	return ::listen(m_ourFd, backLog) == 0;
}

bool TcpServer::accept(uint64_t timeout)
{
	sockaddr_storage addr;
	socklen_t size = sizeof(addr);
	uint64_t start = msystime();
	int fd = -1;
	do {
		fd = ::accept(m_ourFd, (sockaddr *)&addr, &size);
		yield();
	} while(fd < 0 && errno == EAGAIN && (timeout == 0 || msystime() - start < timeout));
	
	if(fd < 0) return false;
	setFd(fd);
	return true;
}

bool TcpServer::makeAvailable()
{
	return true;
}

void TcpServer::endSession()
{
	
}

bool TcpServer::isReliable() const
{
	return true;
}