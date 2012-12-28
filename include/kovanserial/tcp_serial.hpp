#ifndef _TCP_SERIAL_HPP_
#define _TCP_SERIAL_HPP_

#include "tcp.hpp"

#include <netinet/in.h>

class TcpSerial : public Tcp
{
public:
	TcpSerial(const sockaddr_in &addr);
	
	virtual bool makeAvailable();
	void disconnect();
private:
	sockaddr_in m_addr;
};

#endif
