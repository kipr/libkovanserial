#include <kovanserial/kovan_serial.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/serial_comm.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <libgen.h>
#include <sys/time.h>

SerialComm serial;

void sig_handler(int signo)
{
	if(signo != SIGINT) return;
	serial.close();
	exit(EXIT_SUCCESS);
}

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
	
	signal(SIGINT, sig_handler);
	serial.open(argv[1]);
	
	TransportLayer transport(&serial);
	KovanSerial proto(&transport);
	
	if(argc == 4) {
		std::ifstream file(argv[2], std::ios::binary);
		if(!file.is_open()) {
			std::cout << "Couldn't open " << argv[2] << std::endl;
			return EXIT_FAILURE;
		}
		if(!proto.sendFile(std::string(argv[3]) + "/" + basename(argv[2]), &file)) {
			std::cout << "Send failed" << std::endl;
		}
		file.close();
		return EXIT_SUCCESS;
	}
	
	for(;;) {
		Packet p;
		if(!proto.next(p, 1000)) continue;
		if(p.type == Command::FileHeader) {
			long start = msystime();
			Command::FileHeaderData header = *reinterpret_cast<Command::FileHeaderData *>(p.data);
			std::ofstream file(header.dest, std::ios::binary);
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
	
	serial.close();
	return EXIT_SUCCESS;
}