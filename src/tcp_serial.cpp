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
#include <fcntl.h>
#include <unistd.h>
#endif

#include <string.h>
#include <iostream>
#include <stdio.h>

TcpSerial::TcpSerial(const char *host, const unsigned short port)
  : _port(port)
{
	strncpy(_host, host, 128);
}

bool TcpSerial::makeAvailable()
{
	closeFd();
	setFd(socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));
	sockaddr_in service;
	service.sin_family = AF_INET;
  hostent *const he = gethostbyname(_host);
  memcpy(&service.sin_addr, he->h_addr_list[0], he->h_length);
	service.sin_port = htons(_port);
  std::cout << "makeAvail(" << _host << ", " << _port << ")" << std::endl;
	bool ret = ::connect(fd(), reinterpret_cast<const sockaddr *>(&service), sizeof(service));
#ifdef WIN32
	u_long arg = 1;
	if(ioctlsocket(fd(), FIONBIO, &arg) < 0) perror("set non-blocking");
#else
	if(fcntl(fd(), F_SETFL, O_NONBLOCK) < 0) perror("set non-blocking");
#endif
	const int v = 1;
	if(setsockopt(fd(), SOL_SOCKET, SO_REUSEADDR, (const char *)&v, sizeof(v)) < 0) perror("reuseaddr");
	
#ifdef WIN32
	std::cout << "makeAvail ret = " << ret << " " << WSAGetLastError() << std::endl;
#endif
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