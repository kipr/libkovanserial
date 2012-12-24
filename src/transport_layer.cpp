#include "kovanserial/transport_layer.hpp"

#include "kovanserial/serial_comm.hpp"
#include <iostream>

#include <string.h>

Packet::Packet()
	: type(0),
	order(0)
{}

Packet::Packet(const uint16_t &type, const uint32_t &order, const uint8_t *data, const size_t &len)
	: type(type),
	order(order)
{
	if(!data) return;
	memcpy(this->data, data, std::min(len, TRANSPORT_MAX_DATA_SIZE));
}

ChecksummedPacket::ChecksummedPacket()
	: packet(),
	checksum(0)
{}

ChecksummedPacket::ChecksummedPacket(const Packet &packet)
	: packet(packet)
{
	checksum = crc_init();
	checksum = crc_update(checksum, reinterpret_cast<const unsigned char *>(&packet), sizeof(Packet));
	checksum = crc_finalize(checksum);
}

bool ChecksummedPacket::isValid() const
{
	uint32_t c = crc_init();
	c = crc_update(c, reinterpret_cast<const unsigned char *>(&packet), sizeof(Packet));
	c = crc_finalize(c);
	return c == checksum;
}

Ack::Ack()
	: resend(false)
{}

Ack::Ack(const bool &resend)
	: resend(resend)
{
}

TransportLayer::TransportLayer(SerialComm *comm)
	: m_comm(comm)
{
}

TransportLayer::~TransportLayer()
{
}

bool TransportLayer::send(const Packet &p)
{
	ChecksummedPacket ckp(p);
	
	if(!m_comm->write(ckp)) {
		std::cerr << "TransportLayer::send failed to write packet." << std::endl;
		return false;
	}
	
	Ack ack;
	uint8_t tries = 0;
	for(; tries < 10; ++tries) {
		if(!m_comm->read(ack, 1000)) {
			std::cout << "Reading ack failed" << std::endl;
			continue;
		}
		
		if(!ack.resend) break;
		if(!m_comm->write(ckp)) {
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
		if(!m_comm->read(ckp, timeout)) return false;
		ack.resend = !ckp.isValid();
		if(!m_comm->write(ack)) {
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
