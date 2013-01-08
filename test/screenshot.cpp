#include <kovanserial/usb_serial.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/kovan_serial.hpp>

#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
	if(argc != 3) {
		std::cout << argv[0] << " [device] [file.raw565]" << std::endl;
		return EXIT_FAILURE; 
	}
	UsbSerial serial(argv[1]);
	if(!serial.makeAvailable()) {
		std::cout << "makeAvailable failed." << std::endl;
		return EXIT_FAILURE;
	}
	
	TransportLayer transport(&serial);
	KovanSerial proto(&transport);
	
	if(!proto.sendFileAction(COMMAND_ACTION_SCREENSHOT, "")) {
		std::cout << "sendFileAction failed" << std::endl;
		return EXIT_FAILURE;
	}
	
	Packet p;
	if(!proto.next(p, 10000) || p.type != Command::FileHeader) {
		std::cout << "Didn't get file header back." << std::endl;
		return EXIT_FAILURE;
	}
	
	std::ofstream out(argv[2], std::ios_base::binary);
	
	if(!out.is_open()) {
		std::cout << "Couldn't open " << argv[2] << " for writing." << std::endl;
	}
	
	Command::FileHeaderData header;
	p.as(header);
	
	if(!proto.confirmFile(out.is_open()) || !out.is_open()) {
		std::cout << "confirmFile failed" << std::endl;
		return EXIT_FAILURE;
	}
	
	if(!proto.recvFile(header.size, &out, 10000)) {
		std::cout << "recvFile failed" << std::endl;
		return EXIT_FAILURE;
	}
	
	out.close();
	
	proto.hangup();
	
	std::cout << "Done!" << std::endl;
	
	return EXIT_SUCCESS;
}