#include "kovanserial/tcp.hpp"

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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
	return send(m_fd, data, len, 0);
}

ssize_t Tcp::read(uint8_t *data, const size_t &len)
{
	if(!available()) return -1;
	return recv(m_fd, data, len, 0);
}

void Tcp::closeFd()
{
	::close(m_fd);
	m_fd = -1;
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
