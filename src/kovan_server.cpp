#include "kovanserial/kovan_server.hpp"
#include "kovanserial/command_types.hpp"
#include "kovanserial/general.hpp"
#include "kovanserial/kovan_serial.hpp"

#include <fstream>
#include <iostream>

KovanServer::KovanServer(KovanSerial *proto)
	: m_proto(proto)
{
	
}

bool KovanServer::handle(const Packet &p)
{
	if(p.type == Command::FileHeader) handleFile(p);
	else if(p.type == Command::Hangup) return false;
	return true;
}

void KovanServer::handleFile(const Packet &headerPacket)
{
	long start = msystime();
	Command::FileHeaderData header;
	headerPacket.as(header);
	std::ofstream file(header.dest, std::ios::binary);

	if(!m_proto->confirmFile(file.is_open())) return;

	if(!file.is_open()) {
		std::cout << "Couldn't write " << header.dest << std::endl;
		return;
	}

	if(!m_proto->recvFile(header.size, &file, 1000)) {
		std::cout << "recvFile failed" << std::endl;
		return;
	}
	
	file.close();
	long end = msystime();
	std::cout << "Took " << (end - start) << " milliseconds" << std::endl;
}