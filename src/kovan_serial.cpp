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

bool KovanSerial::knockKnock(uint32_t timeout)
{
	char dummy = 0;
	if(m_transport->send(Packet(Command::KnockKnock, dummy)) != Transmitter::Success) return false;
	Packet p;
	return m_transport->recv(p, timeout) == Transmitter::Success && p.type == Command::WhosThere;
}

bool KovanSerial::whosThere()
{
	char dummy = 0;
	return m_transport->send(Packet(Command::WhosThere, dummy)) == Transmitter::Success;
}

bool KovanSerial::hasAuthentication()
{
	return false;
}

bool KovanSerial::sendProperty(const std::string &name, const std::string &value)
{
	Command::PropertyValuePairData data;
	strncpy(data.name, name.c_str(), 8);
	strncpy(data.value, value.c_str(), 128);
	if(m_transport->send(Packet(Command::Property, data)) != Transmitter::Success) {
		std::cout << "Failed to send property" << std::endl;
		return false;
	}
	return true;
}

bool KovanSerial::property(const std::string &name, std::string &value)
{
	Command::PropertyData data;
	strncpy(data.name, name.c_str(), 8);
	if(m_transport->send(Packet(Command::Property, data)) != Transmitter::Success) {
		std::cout << "Failed to send property request" << std::endl;
		return false;
	}
	
	Packet p;
	if(m_transport->recv(p, 1000) != Transmitter::Success || p.type != Command::Property) {
		std::cout << "Did not receive property value pair" << std::endl;
		return false;
	}
	
	Command::PropertyValuePairData ret;
	p.as(ret);
	ret.value[127] = 0; // Make sure we're null termed
	value = ret.value;
	return true;
}

bool KovanSerial::sendPropertyList(const std::list<std::string> &names)
{
	Command::PropertyListData data;
	data.size = names.size();
	
	std::list<std::string>::const_iterator it = names.begin();
	size_t i = 0;
	for(; it != names.end() && i < 50; ++it, ++i) {
		Command::PropertyData entry;
		strncpy(entry.name, (*it).c_str(), 8);
		data.names[i] = entry;
	}
	
	if(m_transport->send(Packet(Command::Property, data)) != Transmitter::Success) {
		std::cout << "Failed to send property" << std::endl;
		return false;
	}
	return true;
}

bool KovanSerial::listProperties(std::list<std::string> &names)
{
	return false;
}

bool KovanSerial::sendFile(const std::string &dest, const std::string &metadata, std::istream *in)
{
	Command::FileHeaderData header;
	strncpy(header.dest, dest.c_str(), 256);
	strncpy(header.metadata, metadata.c_str(), 200);
	in->seekg(0, std::ios::end);
	header.size = in->tellg();
	in->seekg(0, std::ios::beg);
	
	std::cout << "Sending file header." << std::endl;
	if(m_transport->send(Packet(Command::FileHeader, header)) != Transmitter::Success) return false;
	
	std::cout << "Waiting on confirm..." << std::endl;
	Packet confirm;
	if(m_transport->recv(confirm, 15000) != Transmitter::Success || confirm.type != Command::FileConfirm) {
		std::cout << "Didn't receive confirm. Aborting.";
		return false;
	}
	std::cout << "Got confirm packet." << std::endl;
	
	bool good = false;
	confirm.as(good);
	if(!good) {
		std::cout << "Other side rejected our transfer." << std::endl;
		return false;
	}
	std::cout << "Sening file..." << std::endl;
	uint8_t buffer[TRANSPORT_MAX_DATA_SIZE];
	size_t i = 0;
	while(!in->eof() && i < header.size) {
		in->read(reinterpret_cast<char *>(buffer), TRANSPORT_MAX_DATA_SIZE);
		i += in->gcount();
		if(m_transport->send(Packet(Command::File,
			buffer, TRANSPORT_MAX_DATA_SIZE)) != Transmitter::Success) {
			std::cout << "sending file packet failed" << std::endl;
			return false;
		}
		std::cout << "Sent " << i << " of " << header.size << std::endl;
	}
	
	std::cout << "Finished writing entire file" << std::endl;
	
	return true;
}

bool KovanSerial::confirmFile(const bool &good)
{
	return m_transport->send(Packet(Command::FileConfirm, good)) == Transmitter::Success;
}

bool KovanSerial::recvFile(const size_t &size, std::ostream *out, const uint32_t &timeout)
{
	size_t i = 0;
	
	Packet p;
	while(i < size) {
		if(m_transport->recv(p, timeout) != Transmitter::Success) {
			std::cout << "recvFile timed out" << std::endl;
			return false;
		}
		
		if(p.type != Command::File) {
			std::cerr << "Non-file type in file stream." << std::endl;
			return false;
		}
		
		out->write(reinterpret_cast<const char *>(p.data),
			std::min(TRANSPORT_MAX_DATA_SIZE, size - i));
		i += TRANSPORT_MAX_DATA_SIZE;
	}
	
	return true;
}

bool KovanSerial::sendFileAction(const std::string &action, const std::string &file)
{
	Command::FileActionData data;
	strncpy(data.action, action.c_str(), 8);
	strncpy(data.dest, file.c_str(), 500);
	
	if(m_transport->send(Packet(Command::FileAction, data)) != Transmitter::Success) {
		std::cout << "Failed to send file action" << std::endl;
		return false;
	}
	
	Packet confirm;
	if(m_transport->recv(confirm, 10000) != Transmitter::Success || confirm.type != Command::FileActionConfirm) {
		std::cout << "Didn't receive confirm. Aborting." << std::endl;
		return false;
	}
	
	bool good = false;
	confirm.as(good);
	if(!good) {
		std::cout << "Other side rejected our action." << std::endl;
		return false;
	}
	
	return true;
}

bool KovanSerial::confirmFileAction(const bool &good)
{
	return m_transport->send(Packet(Command::FileActionConfirm,
		reinterpret_cast<const uint8_t *>(&good), sizeof(bool))) == Transmitter::Success;
}

bool KovanSerial::sendFileActionProgress(const bool &finished, const double &progress)
{
	Command::FileActionProgressData data;
	data.finished = finished;
	data.progress = progress;
	return m_transport->send(Packet(Command::FileActionProgress, data)) == Transmitter::Success;
}

bool KovanSerial::recvFileActionProgress(bool &finished, double &progress, const uint32_t &timeout)
{
	Packet p;
	if(m_transport->recv(p, timeout) != Transmitter::Success || p.type != Command::FileActionProgress) {
		std::cout << "Couldn't recv file action progress" << std::endl;
		return false;
	}
	
	Command::FileActionProgressData data;
	p.as(data);
	
	finished = data.finished;
	progress = data.progress;
	
	return true;
}

bool KovanSerial::next(Packet &p, const uint32_t &timeout)
{
	return m_transport->recv(p, timeout) == Transmitter::Success;
}

void KovanSerial::hangup()
{
	m_transport->send(Packet(Command::Hangup, 0, 0));
}
