#ifndef _TRANSPORT_COMM_HPP_
#define _TRANSPORT_COMM_HPP_

#define TRANSPORT_MAX_DATA_SIZE ((size_t)512)

#include "crc.h"
#include "transmitter.hpp"
#include "export.h"

#include <algorithm>
#include <string.h>

struct Packet
{
	Packet();
	Packet(const uint16_t &type);
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

class DLL_EXPORT TransportLayer
{
public:
	enum Return {
		Success = 0,
		UntrustedSuccess,
		Error,
		Timeout
	};
	
	TransportLayer(Transmitter *transmitter);
	virtual ~TransportLayer();
	
	void setKey(const uint8_t *key, const uint64_t keySize);
	const uint8_t *key() const;
	uint64_t keySize() const;
	
	virtual Return send(const Packet &p, const bool forceUntrusted = false);
	virtual Return recv(Packet &p, const uint32_t &timeout = 0);
	
private:
	static Return fromTransmitterReturn(const Transmitter::Return ret);
	
	Transmitter *m_transmitter;
	uint64_t m_order;
	
	uint8_t *m_key;
	uint64_t m_keySize;
};

#endif
