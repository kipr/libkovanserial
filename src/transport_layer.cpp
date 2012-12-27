#include "kovanserial/transport_layer.hpp"

#include "kovanserial/transmitter.hpp"
#include <iostream>

#include <string.h>

Packet::Packet()
	: type(0)
{}

Packet::Packet(const uint16_t &type, const uint8_t *data, const size_t &len)
	: type(type)
{
	if(!data) return;
	memcpy(this->data, data, std::min(len, TRANSPORT_MAX_DATA_SIZE));
}

struct ChecksummedPacket
{
	ChecksummedPacket();
	ChecksummedPacket(const Packet &packet, const uint64_t &order);
	
	bool isValid() const;
	
	crc_t computeChecksum() const;
	
	Packet packet;
	uint64_t order;
	crc_t checksum;
};

ChecksummedPacket::ChecksummedPacket()
	: packet(),
	checksum(0)
{}

ChecksummedPacket::ChecksummedPacket(const Packet &packet, const uint64_t &order)
	: packet(packet),
	order(order),
	checksum(computeChecksum())
{
}

bool ChecksummedPacket::isValid() const
{
	return computeChecksum() == checksum;
}

crc_t ChecksummedPacket::computeChecksum() const
{
	crc_t c = crc_init();
	c = crc_update(c, reinterpret_cast<const unsigned char *>(&packet), sizeof(Packet));
	c = crc_update(c, reinterpret_cast<const unsigned char *>(&order), sizeof(uint64_t));
	c = crc_finalize(c);
	return c;
}

struct Ack
{
	Ack();
	Ack(const bool &resend);
	
	bool error : 1;
	bool resend : 1;
};

Ack::Ack()
	: resend(false)
{}

Ack::Ack(const bool &resend)
	: resend(resend)
{
}

TransportLayer::TransportLayer(Transmitter *transmitter)
	: m_transmitter(transmitter)
{
}

TransportLayer::~TransportLayer()
{
}

bool TransportLayer::send(const Packet &p)
{
	ChecksummedPacket ckp(p, m_order++);
	
	if(!m_transmitter->write(ckp)) {
		std::cerr << "TransportLayer::send failed to write packet." << std::endl;
		return false;
	}
	
	Ack ack;
	uint8_t tries = 0;
	for(; tries < 5; ++tries) {
		if(!m_transmitter->read(ack, 1000)) {
			std::cout << "Reading ack failed" << std::endl;
			continue;
		}
		
		if(!ack.resend) break;
		if(!m_transmitter->write(ckp)) {
			std::cout << "Resend failed" << std::endl;
			return false;
		}
		
		std::cout << "resend..." << std::endl;
	}
	
	return tries < 10;
}

bool TransportLayer::recv(Packet &p, const uint32_t &timeout)
{
	ChecksummedPacket ckp;
	Ack ack;
	do {
		if(!m_transmitter->read(ckp, timeout)) return false;
		ack.resend = !ckp.isValid();
		if(!m_transmitter->write(ack)) {
			std::cout << "Writing ack failed" << std::endl;
			return false;
		}
		p = ckp.packet;
		if(ack.resend) {
			std::cout << "Wrote ack with resend = " << ack.resend << std::endl;
		}
	} while(ack.resend);
	
	return true;
}
