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
#include <iostream>

TcpServer::TcpServer()
	: m_ourFd(-1)
{
	m_ourFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if(getaddrinfo(NULL, port, &hints, &res) != 0) return false;
	
	// TODO: This is a temporary windows hack. addrinfo problems?
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(8374);

	int ret = 0;

	
#ifdef WIN32
	ret = ::bind(m_ourFd, (sockaddr *)&service, sizeof(service));
	std::cout << "bind returned " << ret << " on our fd of "
		<< m_ourFd << " " << WSAGetLastError() << std::endl;
#else
	ret = ::bind(m_ourFd, res->ai_addr, res->ai_addrlen);
#endif
	freeaddrinfo(res);
	return ret == 0;
}

bool TcpServer::listen(const int &backLog)
{
	int ret = ::listen(m_ourFd, backLog);
	std::cout << "Listen returned " << ret << std::endl;
	return ret == 0;
}

bool TcpServer::accept(uint64_t timeout)
{
	sockaddr_storage addr;
	socklen_t size = sizeof(addr);
	uint64_t start = msystime();
	int fd = -1;
	do {
		fd = ::accept(m_ourFd, (sockaddr *)&addr, &size);
		// std::cout << "accept = " << WSAGetLastError() << std::endl;
		if(errno == EAGAIN
#ifdef Q_OS_WIN
			|| WSAGetLastError() == WSAEWOULDBLOCK
#endif
			) continue;
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