#ifndef _KOVAN_SERIAL_HPP_
#define _KOVAN_SERIAL_HPP_

#include <iostream>
#include <string>
#include <list>

#include <kovanserial/transport_layer.hpp>
#include <kovanserial/command_types.hpp>

#define KOVAN_SERIAL_PORT "8374"

class KovanSerial
{
public:
	KovanSerial(TransportLayer *transport);
	~KovanSerial();
	
	bool sendProperty(const std::string &name, const std::string &value);
	bool property(const std::string &name, std::string &value);
	
	bool sendPropertyList(const std::list<std::string> &names);
	bool listProperties(std::list<std::string> &names);
	
	bool sendFile(const std::string &dest, std::istream *in);
	bool confirmFile(const bool &good);
	bool recvFile(const size_t &size, std::ostream *out, const uint32_t &timeout = 0);
	
	bool next(Packet &p, const uint32_t &timeout = 0);
	
	void hangup();
	
private:
	TransportLayer *m_transport;
};


#endif
