#include <kovanserial/usb_serial.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/kovan_serial.hpp>
#include <unistd.h>

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
	
	if(!proto.sendFileAction("compile", "test.kar")) {
		std::cout << "sendFileAction failed" << std::endl;
		return EXIT_FAILURE;
	}
	
	double progress = 0.0;
	while(progress < 1.0) {
		if(!proto.sendFileActionProgress(false, progress)) {
			std::cout << "Send file action progress failed" << std::endl;
			return EXIT_FAILURE;
		}
		progress += 0.033;
		sleep(1.0);
	}
	if(!proto.sendFileActionProgress(true, progress)) {
		std::cout << "Send file action progress failed" << std::endl;
		return EXIT_FAILURE;
	}
	
	std::cout << "Done!" << std::endl;
	
	return EXIT_SUCCESS;
}
