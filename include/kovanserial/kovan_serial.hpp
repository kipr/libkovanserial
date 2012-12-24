#ifndef _KOVAN_SERIAL_HPP_
#define _KOVAN_SERIAL_HPP_

#include <iostream>
#include <string>

#include <kovanserial/transport_layer.hpp>
#include <kovanserial/command_types.hpp>

class KovanSerial
{
public:
	KovanSerial(TransportLayer *transport);
	~KovanSerial();
	
	bool sendFile(const std::string &dest, std::istream *in);
	bool recvFile(const size_t &size, std::ostream *out, const uint32_t &timeout = 0);
	
	bool next(Packet &p, const uint32_t &timeout = 0);
	
private:
	TransportLayer *m_transport;
};


#endif
