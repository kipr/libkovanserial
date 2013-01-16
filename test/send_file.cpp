#include <kovanserial/kovan_serial.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/usb_serial.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <libgen.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>

long msystime()
{
	timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1000L + t.tv_usec / 1000L;
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		std::cout << argv[0] << " device [file dest]" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Opening serial port..." << std::endl;
	
	UsbSerial usb(argv[1]);
	if(!usb.makeAvailable()) {
		std::cout << "Failed to open serial port" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "good!" << std::endl;

	TransportLayer transport(&usb);
	KovanSerial proto(&transport);
	

	if(argc == 4) {
		std::cout << "Sending file" << std::endl;
		std::ifstream file(argv[2], std::ios::binary);
		if(!file.is_open()) {
			std::cout << "Couldn't open " << argv[2] << std::endl;
			return EXIT_FAILURE;
		}
		char path[500];
		sprintf(path, "%s/%s", argv[3], argv[2]);
		if(!proto.sendFile(path, "", &file)) {
			std::cout << "Send failed" << std::endl;
		}
		file.close();
		return EXIT_SUCCESS;
	}
	
	std::cout << "Starting as server" << std::endl;
	for(;;) {
		Packet p;
		if(!proto.next(p, 1000)) continue;
		if(p.type == Command::FileHeader) {
			long start = msystime();
			Command::FileHeaderData header;
			memcpy(&header, p.data, sizeof(Command::FileHeaderData));
			std::ofstream file(header.dest, std::ios::binary);
			
			if(!proto.confirmFile(file.is_open())) continue;
			
			if(!file.is_open()) {
				std::cout << "Couldn't write " << header.dest << std::endl;
				continue;
			}
			
			if(!proto.recvFile(header.size, &file, 1000)) {
				std::cout << "recvFile failed" << std::endl;
				continue;
			}
			file.close();
			long end = msystime();
			std::cout << "Took " << (end - start) <<" milliseconds" << std::endl;
		}
		std::cout << "type = " << p.type << std::endl;
	}
	
	usb.endSession();
	return EXIT_SUCCESS;
}