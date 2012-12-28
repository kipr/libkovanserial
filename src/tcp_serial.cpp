#include "kovanserial/tcp_serial.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

TcpSerial::TcpSerial(const sockaddr_in &addr)
	: m_addr(addr)
{
	setFd(socket(PF_INET, SOCK_STREAM, 0));
}

bool TcpSerial::makeAvailable()
{
	disconnect();
	return ::connect(fd(), (sockaddr *)&m_addr, sizeof(m_addr)) == 0;
}

void TcpSerial::disconnect()
{
	closeFd();
}