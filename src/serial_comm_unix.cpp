#include "kovanserial/serial_comm.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

SerialComm::SerialComm()
	: m_fd(-1)
{
	
}

SerialComm::~SerialComm()
{
	close();
}

bool SerialComm::open(const char *dev)
{
	if(m_fd >= 0) close();
	m_fd = ::open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	const bool success = m_fd >= 0;
	if(!success) return false;
	tcflush(m_fd, TCIOFLUSH);
	configure();
	return true;
}

void SerialComm::close()
{
	if(m_fd < 0) return;
	::close(m_fd);
	m_fd = -1;
}

bool SerialComm::isOpen() const
{
	return m_fd >= 0;
}

ssize_t SerialComm::write(const uint8_t *data, const size_t &len)
{
	return ::write(m_fd, data, len);
}

ssize_t SerialComm::read(uint8_t *data, const size_t &len)
{
	return ::read(m_fd, data, len);
}

SerialComm::SerialComm(const SerialComm &rhs)
{
}

void SerialComm::configure()
{
	if(m_fd < 0) return;
	struct ::termios t;
	tcgetattr(m_fd, &t);

	t.c_oflag = 0;
	t.c_lflag = 0;
	t.c_iflag = IGNBRK | IGNPAR;
	t.c_cflag &= ~PARENB;
	t.c_cflag &= ~CSTOPB;
	t.c_cflag &= ~CSIZE;
	t.c_cflag |= CS8;
	t.c_cflag |= CREAD;
	t.c_cflag |= CLOCAL;
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	cfsetispeed(&t, 115200);
	cfsetospeed(&t, 115200);
	tcsetattr(m_fd, TCSANOW, &t);
}