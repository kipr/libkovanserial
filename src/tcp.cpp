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

ssize_t Tcp::readBlock(uint8_t *data, const size_t len, const uint32_t timeout)
{
	if(!available()) return -1;
	size_t pos = 0;
	long startTime = msystime();
	do {
		long endTime = msystime();
		if(timeout > 0 && endTime - startTime > timeout) return pos;

		ssize_t ret = read(data + pos, len - pos);
		if(ret < 0
#ifdef WIN32
			&& WSAGetLastError() != WSAEWOULDBLOCK
#else
			&& errno != EAGAIN
#endif
			) return ret;
		if(ret > 0) {
			pos += ret;
			startTime = endTime;
		}
		yield();
	} while(pos < len);

	return pos;
}

ssize_t Tcp::write(const uint8_t *data, const size_t len)
{
	if(!available()) return -1;
	return send(m_fd, reinterpret_cast<const char *>(data), len, 0);
}

ssize_t Tcp::read(uint8_t *data, const size_t len)
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
