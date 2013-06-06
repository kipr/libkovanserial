#include "kovanserial/transport_layer.hpp"

#include "kovanserial/transmitter.hpp"
#include "kovanserial/xor.hpp"
#include "kovanserial/randomize.hpp"
#include <iostream>

#include "kovanserial/md5.hpp"

#include <string.h>

Packet::Packet()
	: type(0)
{}

Packet::Packet(const uint16_t &type)
	: type(type)
{
	randomize::fill(data, TRANSPORT_MAX_DATA_SIZE);
}

Packet::Packet(const uint16_t &type, const uint8_t *data, const size_t &len)
	: type(type)
{
	randomize::fill(this->data, TRANSPORT_MAX_DATA_SIZE);
	if(!data) return;
	memcpy(this->data, data, std::min(len, TRANSPORT_MAX_DATA_SIZE));
} 

struct ChecksummedPacket
{
	ChecksummedPacket();
	ChecksummedPacket(const Packet &packet, const uint64_t &order);
	ChecksummedPacket(const Packet &packet, const uint64_t &order, const uint8_t *const key,
		const uint64_t keySize);
	
	bool isValid() const;
	bool isEncrypted() const;
	void decrypt(const uint8_t *const key, const uint64_t keySize);
	
	crc_t computeChecksum() const;
	
	Packet packet;
	uint8_t encrypted;
	uint8_t unused[15];
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
	checksum(computeChecksum()),
	encrypted(0)
{
	randomize::fill(unused, sizeof(unused));
}

ChecksummedPacket::ChecksummedPacket(const Packet &packet, const uint64_t &order, const uint8_t *const key,
	const uint64_t keySize)
	: packet(packet),
	order(order),
	checksum(computeChecksum()),
	encrypted(1)
{
	xor_crypt::crypt(this->packet, key, keySize);
	randomize::fill(unused, sizeof(unused));
}

bool ChecksummedPacket::isValid() const
{
	return computeChecksum() == checksum;
}

bool ChecksummedPacket::isEncrypted() const
{
	return encrypted ? true : false;
}

void ChecksummedPacket::decrypt(const uint8_t *const key, const uint64_t keySize)
{
	if(!encrypted || !key) return;
	xor_crypt::crypt(packet, key, keySize);
	encrypted = 0;
}

crc_t ChecksummedPacket::computeChecksum() const
{
	crc_t c = crc_init();
	c = crc_update(c, reinterpret_cast<const uint8_t *>(&packet), sizeof(Packet));
	c = crc_update(c, reinterpret_cast<const uint8_t *>(&order), sizeof(uint64_t));
	c = crc_finalize(c);
	return c;
}

struct Ack
{
	Ack();
	Ack(const bool &resend);
	
	bool resend : 1;
	bool rejected : 1;
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
	m_key(0),
	m_keySize(0)
{
}

TransportLayer::~TransportLayer()
{
}

void TransportLayer::setKey(const uint8_t *key, const uint64_t size)
{
	delete[] m_key;
	m_key = 0;
	m_keySize = 0;
	if(!key || size < 1) return;
	m_keySize = size;
	m_key = new uint8_t[size];
	memcpy(m_key, key, size);
}

const uint8_t *TransportLayer::key() const
{
	return m_key;
}

uint64_t TransportLayer::keySize() const
{
	return m_keySize;
}

TransportLayer::Return TransportLayer::send(const Packet &p)
{
	MD5 tmp;
	tmp.update(reinterpret_cast<const uint8_t *>(&p), sizeof(p));
	tmp.finalize();
	
	std::cout << "Sent packet MD5: ";
	for(unsigned i = 0; i < 16; ++i) {
		std::cout << std::hex << (uint16_t)tmp.digest()[i];
	}
	std::cout << std::endl;
	
	// If we have a key, encrypt the packet
	ChecksummedPacket ckp = m_key
		? ChecksummedPacket(p, m_order, m_key, m_keySize)
		: ChecksummedPacket(p, m_order);
	++m_order;
	
	if(m_transmitter->write(ckp) != Transmitter::Success) {
		std::cerr << "TransportLayer::send failed to write packet." << std::endl;
		return TransportLayer::Error;
	}
	
	if(m_transmitter->isReliable()) return TransportLayer::Success;
	
	Ack ack;
	uint8_t tries = 0;
	for(; tries < 5; ++tries) {
		std::cout << "Waiting on ack..." << std::endl;
		Transmitter::Return ret = m_transmitter->read(ack, 1000);
		if(ret == Transmitter::Timeout) {
			std::cout << "Reading ack failed" << std::endl;
			continue;
		}
		if(ret != Transmitter::Success) return fromTransmitterReturn(ret);
		
		std::cout << "Got an ack!" << std::endl;
		if(!ack.resend) break;
		if(m_transmitter->write(ckp) != Transmitter::Success) {
			std::cout << "Resend failed" << std::endl;
			return TransportLayer::Error;
		}
		
		std::cout << "resend..." << std::endl;
	}
	
	std::cout << "Send finished with tries = " << (int)tries << std::endl;
	
	return fromTransmitterReturn((tries < 5) ? Transmitter::Success : Transmitter::Timeout);
}

TransportLayer::Return TransportLayer::recv(Packet &p, const uint32_t &timeout)
{
	ChecksummedPacket ckp;
	Ack ack;
	bool encrypted = false;
	do {
		Transmitter::Return ret = m_transmitter->read(ckp, timeout);
		std::cout << ret << std::endl;
		if(ret != Transmitter::Success) {
			std::cout << "Reading ckp failed." << std::endl;
			return fromTransmitterReturn(ret);
		}
		
		// Decrypt must be called before other operations
		encrypted = ckp.isEncrypted();
		ckp.decrypt(m_key, m_keySize);
		p = ckp.packet;
		
		MD5 tmp;
		tmp.update(reinterpret_cast<const uint8_t *>(&p), sizeof(p));
		tmp.finalize();
	
		std::cout << "Recved packet MD5: ";
		for(unsigned i = 0; i < 16; ++i) {
			std::cout << std::hex << (uint16_t)tmp.digest()[i];
		}
		std::cout << std::endl;
		
		if(m_transmitter->isReliable()) {
			std::cout << "Packet delivered. Encrypted? " << encrypted << std::endl;
			return encrypted
				? TransportLayer::Success
				: TransportLayer::UntrustedSuccess;
		}
		
		ack.resend = !ckp.isValid();
		if(m_transmitter->write(ack) != Transmitter::Success) {
			std::cout << "Writing ack failed" << std::endl;
			return TransportLayer::Error;
		}
		if(ack.resend) {
			std::cout << "Wrote ack with resend = " << ack.resend << " (got packet "
				<< p.type << ")" << std::endl;
		}
	} while(ack.resend);
	
	return encrypted
		? TransportLayer::Success
		: TransportLayer::UntrustedSuccess;
}

TransportLayer::Return TransportLayer::fromTransmitterReturn(const Transmitter::Return ret)
{
	switch(ret) {
	case Transmitter::Success: return TransportLayer::Success;
	case Transmitter::Error: return TransportLayer::Error;
	case Transmitter::Timeout: return TransportLayer::Timeout;
	}
	return TransportLayer::Error;
}