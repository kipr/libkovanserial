#include "kovanserial/tcp_serial.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

TcpSerial::TcpSerial()
{
	setFd(socket(PF_INET, SOCK_STREAM, 0));
}

bool TcpSerial::connect(const char *host, const char *service)
{
	addrinfo hints;
	addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(host, service, &hints, &res);
	bool ret = ::connect(fd(), res->ai_addr, res->ai_addrlen) == 0;
	freeaddrinfo(res);
	return ret;
}

void TcpSerial::disconnect()
{
	closeFd();
}