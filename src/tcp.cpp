#include "kovanserial/tcp.hpp"

#include <unistd.h>

#include "socket_utils.hpp"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

Tcp::Tcp()
	: m_fd(-1)
{
}

Tcp::~Tcp()
{
	closeFd();
}

bool Tcp::available() const
{
	return m_fd >= 0;
}

ssize_t Tcp::write(const uint8_t *data, const size_t &len)
{
	if(!available()) return -1;
	return send(m_fd, reinterpret_cast<const char *>(data), len, 0);
}

ssize_t Tcp::read(uint8_t *data, const size_t &len)
{
	if(!available()) return -1;
	return recv(m_fd, reinterpret_cast<char *>(data), len, 0);
}

void Tcp::closeFd()
{
	closeSocket(m_fd);
}

void Tcp::setFd(const int &fd)
{
	if(m_fd != fd) closeFd();
	m_fd = fd;
}

const int &Tcp::fd() const
{
	return m_fd;
}
