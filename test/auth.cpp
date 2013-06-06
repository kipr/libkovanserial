#include <iostream>
#include <fstream>
#include <sstream>

#include <kovanserial/tcp_serial.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/kovan_serial.hpp>

int main(int argc, char *argv[])
{
	TcpSerial serial("127.0.0.1", KOVAN_SERIAL_PORT);
	if(!serial.makeAvailable()) {
		std::cerr << "Make available failed" << std::endl;
		return 1;
	}
	
	TransportLayer transport(&serial);
	KovanSerial proto(&transport);
	
	bool authNecessary = false;
	if(!proto.authenticationInfo(authNecessary)) {
		std::cerr << "Failed to fetch authorization info." << std::endl;
		serial.endSession();
		return 1;
	}
	proto.hangup();
	
	if(!authNecessary) {
		std::cout << "Authorized (No authorization necessary)" << std::endl;
		
		return 0;
	}
	
	bool authed = false;
	do {
		std::cout << "Please enter password: (or 'q' to quit)" << std::endl;
	
		std::string password;
		std::cin >> password;
		
		if(password.compare("q") == 0) break;
		
		proto.setPassword(password);
		std::cout << "MD5: ";
		for(unsigned char i = 0; i < 16; ++i) {
			std::cout << std::hex << (uint16_t)proto.passwordMd5()[i];
		}
		std::cout << std::endl;
	
		std::cout << "SHA1: ";
		for(unsigned char i = 0; i < 20; ++i) {
			std::cout << std::hex << (uint16_t)proto.passwordSha1()[i];
		}
		std::cout << std::endl;
	
		serial.endSession();
		if(!serial.makeAvailable()) {
			std::cerr << "Make available failed" << std::endl;
			return 1;
		}
		uint8_t sessionKey[KOVAN_SERIAL_SESSION_KEY_SIZE];
		memset(sessionKey, 0, KOVAN_SERIAL_SESSION_KEY_SIZE);
		if(!proto.requestAuthentication(authed, sessionKey) || !authed) {
			std::cout << "Authorization failed. Try again." <<std::endl;
		} else {
			std::cout << "Decrypted session key: ";
			for(unsigned char i = 0; i < KOVAN_SERIAL_SESSION_KEY_SIZE; ++i) {
				std::cout << std::hex << (uint16_t)sessionKey[i];
			}
			std::cout << std::endl;
		}
	} while(!authed);
	
	Packet p;
	std::ofstream sstream("test.pdf");
	Command::FileHeaderData header;
	bool finished = false;
	// TODO: Show progress
	double progress = 0.0;
	
	if(!authed) goto end;
	
	std::cout << "Authorized" << std::endl;
	
	if(!proto.sendFileAction(COMMAND_ACTION_READ, "/Users/beta/Books/Mathematics/Discrete Mathematics 7th Edition - Rosen.pdf")) {
		goto end;
	}
	
	if(transport.recv(p, 15000) != TransportLayer::Success || p.type != Command::FileHeader) {
		std::cout << "Failed to recv results of compile" << p.type << std::endl;
		goto end;
	}
	
	p.as(header);
	if(!proto.confirmFile(true)) {
		std::cout << "Failed to confirm incoming compile results" << std::endl;
		goto end;
	};
	
	
	if(!proto.recvFile(header.size, &sstream, 5000)) {
		std::cout << "Recv compile results failed" << std::endl;
		goto end;
	}
	
	
	
	
	end:
	
	proto.hangup();
	serial.endSession();
	
	return 0;
}