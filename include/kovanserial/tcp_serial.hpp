#ifndef _TCP_SERIAL_HPP_
#define _TCP_SERIAL_HPP_

#include "tcp.hpp"

#include <sys/socket.h>

class TcpSerial : public Tcp
{
public:
	TcpSerial(const char *host, const char *service);
	
	virtual bool makeAvailable();
	virtual void endSession();
	virtual bool isReliable() const;

private:
	char m_host[128];
	char m_service[128];
};

#endif
