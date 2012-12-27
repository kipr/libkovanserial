#ifndef _TCP_SERIAL_HPP_
#define _TCP_SERIAL_HPP_

#include "tcp.hpp"

class TcpSerial : public Tcp
{
public:
	TcpSerial();
	
	bool connect(const char *host, const char *service);
	void disconnect();
};

#endif
