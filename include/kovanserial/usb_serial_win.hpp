#ifndef _USB_SERIAL_WIN_HPP_
#define _USB_SERIAL_WIN_HPP_

#include "transmitter.hpp"

#include <stdint.h>
#include <windows.h>

class UsbSerialWin : public Transmitter
{
public:
	UsbSerialWin(const char *dev);
	~UsbSerialWin();
	
	virtual bool makeAvailable();
	virtual void endSession();
	virtual bool isReliable() const;
	virtual bool available() const;
	
	virtual ssize_t readBlock(uint8_t *data, const size_t len, const uint32_t timeout = 0);
	
	virtual ssize_t write(const uint8_t *data, const size_t len);
	virtual ssize_t read(uint8_t *data, const size_t len);
	
private:
	void configure();
	
	HANDLE m_handle;
	char m_dev[128];
};

#endif
