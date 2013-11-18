#include "kovanserial/tcp_server.hpp"

#include "kovanserial/general.hpp"
#include "kovanserial/socket_utils.hpp"

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#define NOMINMAX
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>

TcpServer::TcpServer()
	: m_ourFd(-1)
	, m_restriction(TcpServer::None)
{
	m_ourFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setBlocking(m_ourFd, false);
}

TcpServer::~TcpServer()
{
	if(m_ourFd < 0) return;
	closeSocket(m_ourFd);

}

bool TcpServer::bind(const unsigned short port)
{
	// TODO: This is a temporary windows hack. addrinfo problems?
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(port);

	int ret = 0;
	
#ifdef WIN32
	ret = ::bind(m_ourFd, (sockaddr *)&service, sizeof(service));
	std::cout << "bind returned " << ret << " on our fd of "
		<< m_ourFd << " " << WSAGetLastError() << std::endl;
#else
	ret = ::bind(m_ourFd, reinterpret_cast<const sockaddr *>(&service), sizeof(service));
#endif
	
#ifdef WIN32
	u_long arg = 1;
	if(ioctlsocket(m_ourFd, FIONBIO, &arg) < 0) perror("set non-blocking");
#else
	if(fcntl(m_ourFd, F_SETFL, O_NONBLOCK) < 0) perror("set non-blocking");
#endif

	const int v = 1;
	if(setsockopt(m_ourFd, SOL_SOCKET, SO_REUSEADDR, (const char *)&v, sizeof(v)) < 0) perror("reuseaddr");
	
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
		if(m_restriction == OnlyLocal && fd >= 0) {
			sockaddr_in6 peer;
			socklen_t len = sizeof(peer);
			getpeername(fd, (sockaddr *)&peer, &len);
			sockaddr_in6 localhost;
			memset(localhost.sin6_addr.s6_addr, 0, 16);
			localhost.sin6_addr.s6_addr[15] = 1;
      if(!memcmp(peer.sin6_addr.s6_addr, localhost.sin6_addr.s6_addr, 16)) {
        close(fd);
        continue;
      }
		}
		
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

void TcpServer::setConnectionRestriction(const TcpServer::ConnectionRestriction restriction)
{
	m_restriction = restriction;
}

TcpServer::ConnectionRestriction TcpServer::connectionRestriction() const
{
	return m_restriction;
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