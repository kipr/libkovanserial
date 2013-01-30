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
	ChecksummedPacket(const Packet &packet, const uint64_t &order, const uint8_t *password = 0);
	
	bool isValid() const;
	bool isPasswordGood(const uint8_t *password) const;
	
	crc_t computeChecksum() const;
	
	Packet packet;
	uint8_t password[16];
	uint64_t order;
	crc_t checksum;
};

ChecksummedPacket::ChecksummedPacket()
	: packet(),
	checksum(0)
{}

ChecksummedPacket::ChecksummedPacket(const Packet &packet, const uint64_t &order, const uint8_t *password)
	: packet(packet),
	order(order),
	checksum(computeChecksum())
{
	if(!password) return;
	memcpy(this->password, password, 16);
}

bool ChecksummedPacket::isValid() const
{
	return computeChecksum() == checksum;
}

bool ChecksummedPacket::isPasswordGood(const uint8_t *password) const
{
	return memcmp(this->password, password, 16) == 0;
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
	: m_transmitter(transmitter),
	m_authMode(TransportLayer::AuthClient)
{
}

TransportLayer::~TransportLayer()
{
}

void TransportLayer::setAuthMode(TransportLayer::AuthMode authMode)
{
	m_authMode = authMode;
}

TransportLayer::AuthMode TransportLayer::authMode() const
{
	return m_authMode;
}

void TransportLayer::setPassword(const uint8_t *password)
{
	memcpy(m_password, password, 16);
}

const uint8_t *TransportLayer::password() const
{
	return m_password;
}

Transmitter::Return TransportLayer::send(const Packet &p)
{
	// std::cout << "Creating checksummed packet" << std::endl;
	ChecksummedPacket ckp(p, m_order++, m_authMode == TransportLayer::AuthClient ? m_password : 0);
	
	// std::cout << "Writing packet" << std::endl;
	if(m_transmitter->write(ckp) != Transmitter::Success) {
		std::cerr << "TransportLayer::send failed to write packet." << std::endl;
		return Transmitter::Error;
	}
	// std::cout << "Finished writing packet!" << std::endl;
	
	if(m_transmitter->isReliable()) return Transmitter::Success;
	
	Ack ack;
	uint8_t tries = 0;
	for(; tries < 5; ++tries) {
		std::cout << "Waiting on ack..." << std::endl;
		Transmitter::Return ret = m_transmitter->read(ack, 1000);
		if(ret == Transmitter::Timeout) {
			std::cout << "Reading ack failed" << std::endl;
			continue;
		}
		if(ret != Transmitter::Success) return ret;
		
		std::cout << "Got an ack!" << std::endl;
		if(!ack.resend) break;
		if(m_transmitter->write(ckp) != Transmitter::Success) {
			std::cout << "Resend failed" << std::endl;
			return Transmitter::Error;
		}
		
		std::cout << "resend..." << std::endl;
	}
	
	std::cout << "Send finished with tries = " << (int)tries << std::endl;
	
	return (tries < 5) ? Transmitter::Success : Transmitter::Timeout;
}

Transmitter::Return TransportLayer::recv(Packet &p, const uint32_t &timeout)
{
	ChecksummedPacket ckp;
	Ack ack;
	do {
		Transmitter::Return ret = m_transmitter->read(ckp, timeout);
		if(ret != Transmitter::Success) {
			std::cout << "Reading ckp failed." << std::endl;
			return ret;
		}
		if(m_authMode == TransportLayer::AuthServer && !ckp.isPasswordGood(m_password)) {
			std::cout << "Passwords don't match" << std::endl;
			return Transmitter::Error;
		}
		p = ckp.packet;
		if(m_transmitter->isReliable()) return Transmitter::Success;
		ack.resend = !ckp.isValid();
		if(m_transmitter->write(ack) != Transmitter::Success) {
			std::cout << "Writing ack failed" << std::endl;
			return Transmitter::Error;
		}
		if(ack.resend) {
			std::cout << "Wrote ack with resend = " << ack.resend << " (got packet "
				<< p.type << ")" << std::endl;
		}
	} while(ack.resend);
	
	return Transmitter::Success;
}
