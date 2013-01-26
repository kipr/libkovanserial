#include <kovanserial/usb_serial.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/kovan_serial.hpp>
#include <kovanserial/command_types.hpp>

#include <iostream>

int main(int argc, char *argv[])
{
	if(argc != 2) {
		std::cout << argv[0] << " device" << std::endl;
		return EXIT_FAILURE; 
	}
	UsbSerial serial(argv[1]);
	if(!serial.makeAvailable()) {
		std::cout << "makeAvailable failed." << std::endl;
		return EXIT_FAILURE;
	}
	
	TransportLayer transport(&serial);
	KovanSerial proto(&transport);
	
	Packet p;
	if(!proto.next(p, 10000) || p.type != Command::FileAction) {
		std::cout << "Didn't receive file action" << std::endl;
		return EXIT_FAILURE;
	}
	
	Command::FileActionData data;
	p.as(data);
	std::cout << data.action << " on " << data.dest << std::endl;
	
	if(!proto.confirmFileAction(true)) {
		std::cout << "Couldn't confirm" << std::endl;
		return EXIT_FAILURE;
	}
	
	bool finished = false;
	double progress = 0.0;
	do {
		finished = progress >= 1.0;
		if(!proto.recvFileActionProgress(finished, progress)) {
			std::cout << "Send file action progress failed" << std::endl;
			return EXIT_FAILURE;
		}
		std::cout << "progress: " << progress << std::endl;
	} while(!finished);
	
	std::cout << "Done!" << std::endl;
	
	return EXIT_SUCCESS;
}