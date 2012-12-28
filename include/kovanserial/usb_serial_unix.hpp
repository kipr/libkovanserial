#ifndef _USB_SERIAL_UNIX_HPP_
#define _USB_SERIAL_UNIX_HPP_

#include "transmitter.hpp"

class UsbSerialUnix : public Transmitter
{
public:
	UsbSerialUnix(const char *dev);
	~UsbSerialUnix();
	
	virtual bool makeAvailable();
	virtual void close();
	virtual bool available() const;
	
	virtual ssize_t write(const uint8_t *data, const size_t &len);
	virtual ssize_t read(uint8_t *data, const size_t &len);
	
private:
	void configure();
	
	int m_fd;
	char m_dev[128];
};

#endif
