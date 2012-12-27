#include <kovanserial/tcp_serial.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/kovan_serial.hpp>

#include <fstream>

int main(int argc, char *argv[])
{
	if(argc != 4) {
		std::cout << argv[0] << " host file dest" << std::endl;
		return EXIT_FAILURE;
	}
	
	TcpSerial client;
	TransportLayer transport(&client);
	KovanSerial proto(&transport);
	
	if(!client.connect(argv[1], KOVAN_SERIAL_PORT)) {
		std::cout << "Failed to connect" << std::endl;
	}
	
	std::ifstream file(argv[2], std::ios::binary);
	if(!file.is_open()) {
		std::cout << "Couldn't open " << argv[2] << std::endl;
		return EXIT_FAILURE;
	}
	
	char path[500];
	sprintf(path, "%s/%s", argv[3], argv[2]);
	if(!proto.sendFile(path, &file)) {
		std::cout << "Send failed" << std::endl;
	}
	file.close();
	proto.hangup();
	
	return EXIT_SUCCESS;
}