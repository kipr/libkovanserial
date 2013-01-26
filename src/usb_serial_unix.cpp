#include "kovanserial/usb_serial_unix.hpp"


#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

UsbSerialUnix::UsbSerialUnix(const char *dev)
	: m_fd(-1)
{
	strncpy(m_dev, dev, 128);
}

UsbSerialUnix::~UsbSerialUnix()
{
	close();
}

bool UsbSerialUnix::makeAvailable()
{
	if(m_fd >= 0) return true;
	m_fd = ::open(m_dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	const bool success = m_fd >= 0;
	if(!success) {
		perror("open");
		return false;
	}
	tcflush(m_fd, TCIOFLUSH);
	configure();
	return true;
}

void UsbSerialUnix::endSession()
{
	close();
}

bool UsbSerialUnix::isReliable() const
{
	return false;
}

void UsbSerialUnix::close()
{
	if(m_fd < 0) return;
	::close(m_fd);
	m_fd = -1;
}

bool UsbSerialUnix::available() const
{
	return m_fd >= 0;
}

ssize_t UsbSerialUnix::readBlock(uint8_t *data, const size_t len, const uint32_t timeout)
{
	if(!available()) return -1;
	size_t pos = 0;
	long startTime = msystime();
	do {
		long endTime = msystime();
		if(timeout > 0 && endTime - startTime > timeout) return pos;

		ssize_t ret = read(data + pos, len - pos);
		if(ret < 0 && errno != EAGAIN) return ret;
		if(ret > 0) {
			pos += ret;
			startTime = endTime;
		}
		yield();
	} while(pos < len);

	return pos;
}

ssize_t UsbSerialUnix::write(const uint8_t *data, const size_t len)
{
	return ::write(m_fd, data, len);
}

ssize_t UsbSerialUnix::read(uint8_t *data, const size_t len)
{
	return ::read(m_fd, data, len);
}

void UsbSerialUnix::configure()
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