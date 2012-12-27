#ifndef _KOVAN_SERVER_HPP_
#define _KOVAN_SERVER_HPP_

#include "transport_layer.hpp"

class KovanSerial;

class KovanServer
{
public:
	KovanServer(KovanSerial *proto);
	bool handle(const Packet &p);
	
private:
	void handleFile(const Packet &headerPacket);
	KovanSerial *m_proto;
};

#endif
