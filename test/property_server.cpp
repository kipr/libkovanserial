#include <kovanserial/kovan_serial.hpp>
#include <kovanserial/usb_serial.hpp>
#include <kovanserial/transport_layer.hpp>

#include <string>
#include <map>

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
	if(!proto.next(p, 10000) || p.type != Command::Property) {
		std::cout << "Didn't receive property" << std::endl;
		return EXIT_FAILURE;
	}
	
	Command::PropertyData data;
	p.as(data);
	std::cout << "\"" << data.name << "\"" << std::endl;
	
	std::map<std::string, std::string> properties;
	properties["dsnm"] = "Kovan";
	
	if(!proto.sendProperty(data.name, properties[data.name])) {
		std::cout << "Couldn't send property" << std::endl;
		return EXIT_FAILURE;
	}
	
	std::cout << "Done!" << std::endl;
	
	return EXIT_SUCCESS;
}