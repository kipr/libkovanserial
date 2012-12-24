#include "kovanserial/kovan_serial.hpp"
#include "kovanserial/transport_layer.hpp"

#include <string.h>

KovanSerial::KovanSerial(TransportLayer *transport)
	: m_transport(transport)
{
	
}

KovanSerial::~KovanSerial()
{
	
}

bool KovanSerial::sendFile(const std::string &dest, std::istream *in)
{
	Command::FileHeaderData header;
	strncpy(header.dest, dest.c_str(), 4096);
	in->seekg(0, std::ios::end);
	header.size = in->tellg();
	in->seekg(0, std::ios::beg);
	if(!m_transport->send(Packet(Command::FileHeader, 0,
		reinterpret_cast<uint8_t *>(&header),
		sizeof(Command::FileHeaderData)))) return false;
	uint32_t order = 0;
	uint8_t buffer[TRANSPORT_MAX_DATA_SIZE];
	while(!in->eof() && !in->fail()) {
		in->read(reinterpret_cast<char *>(buffer), TRANSPORT_MAX_DATA_SIZE);
		if(!m_transport->send(Packet(Command::File, ++order,
			buffer, TRANSPORT_MAX_DATA_SIZE))) return false;
	}
	
	std::cout << "Finished writing entire file" << std::endl;
	
	return !in->fail();
}

bool KovanSerial::recvFile(const size_t &size, std::ostream *out, const uint32_t &timeout)
{
	size_t i = 0;
	
	Packet p;
	while(i < size) {
		if(!m_transport->recv(p, timeout)) return false;
		if(p.type != Command::File) {
			std::cerr << "Non-file type in file stream." << std::endl;
			return false;
		}
		std::cout << "Got chunk " << (i / TRANSPORT_MAX_DATA_SIZE) << " of "<< (size / TRANSPORT_MAX_DATA_SIZE) << std::endl;
		out->write(reinterpret_cast<const char *>(p.data),
			std::min(TRANSPORT_MAX_DATA_SIZE, size - i));
		i += TRANSPORT_MAX_DATA_SIZE;
	}
	
	return true;
}

bool KovanSerial::next(Packet &p, const uint32_t &timeout)
{
	return m_transport->recv(p, timeout);
}
