#include <kovanserial/tcp_server.hpp>
#include <kovanserial/transport_layer.hpp>
#include <kovanserial/kovan_serial.hpp>

#include <fstream>

int main(int argc, char *argv[])
{
	TcpServer server;
	TransportLayer transport(&server);
	KovanSerial proto(&transport);
	
	server.bind("1234");
	server.listen(1);
	while(server.accept(0)) {
		for(;;) {
			Packet p;
			if(!proto.next(p, 5000)) break;
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
				std::cout << "Took " << (end - start) << " milliseconds" << std::endl;
			}
			if(p.type == Command::Hangup) break;
		}
		std::cout << "Waiting for new connection..." << std::endl;
	}
	
	return 0;
}