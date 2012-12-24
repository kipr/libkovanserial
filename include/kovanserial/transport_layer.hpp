#ifndef _TRANSPORT_COMM_HPP_
#define _TRANSPORT_COMM_HPP_

#define TRANSPORT_MAX_DATA_SIZE ((size_t)512)

#include "crc.h"

struct Packet
{
	Packet();
	Packet(const uint16_t &type, const uint32_t &order, const uint8_t *data, const size_t &len);
	
	uint16_t type;
	uint32_t order;
	uint8_t data[TRANSPORT_MAX_DATA_SIZE];
};

struct ChecksummedPacket
{
	ChecksummedPacket();
	ChecksummedPacket(const Packet &packet);
	
	bool isValid() const;
	
	Packet packet;
	crc_t checksum;
};

struct Ack
{
	Ack();
	Ack(const bool &resend);
	
	bool error : 1;
	bool resend : 1;
};

class SerialComm;

class TransportLayer
{
public:
	TransportLayer(SerialComm *comm);
	~TransportLayer();
	
	virtual bool send(const Packet &p);
	virtual bool recv(Packet &p, const uint32_t &timeout = 0);
	
private:
	SerialComm *m_comm;
};

#endif
