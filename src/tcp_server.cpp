#include "kovanserial/tcp_server.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

TcpServer::TcpServer()
	: m_ourFd(-1)
{
	m_ourFd = socket(PF_INET, SOCK_STREAM, 0);
}

TcpServer::~TcpServer()
{
	if(m_ourFd >= 0) ::close(m_ourFd);
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

bool TcpServer::accept()
{
	sockaddr_storage addr;
	socklen_t size = sizeof(addr);
	int fd = ::accept(m_ourFd, (sockaddr *)&addr, &size);
	if(fd < 0) return false;
	setFd(fd);
	return true;
}

bool TcpServer::makeAvailable()
{
	return true;
}