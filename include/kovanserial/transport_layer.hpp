#ifndef _TRANSPORT_COMM_HPP_
#define _TRANSPORT_COMM_HPP_

#define TRANSPORT_MAX_DATA_SIZE ((size_t)512)

#include "crc.h"

#include <algorithm>
#include <string.h>

struct Packet
{
	Packet();
	Packet(const uint16_t &type, const uint8_t *data, const size_t &len);
	
	template<typename T>
	Packet(const uint16_t &type, const T &data)
		: type(type)
	{
		memcpy(this->data, &data, std::min(sizeof(T), TRANSPORT_MAX_DATA_SIZE));
	}
	
	template<typename T>
	void as(T &t) const
	{
		memcpy(reinterpret_cast<uint8_t *>(&t), data,
			std::min(sizeof(T), TRANSPORT_MAX_DATA_SIZE));
	}
	
	uint16_t type;
	uint8_t data[TRANSPORT_MAX_DATA_SIZE];
};

class Transmitter;

class TransportLayer
{
public:
	TransportLayer(Transmitter *transmitter);
	~TransportLayer();
	
	virtual bool send(const Packet &p);
	virtual bool recv(Packet &p, const uint32_t &timeout = 0);
	
private:
	Transmitter *m_transmitter;
	uint64_t m_order;
};

#endif
