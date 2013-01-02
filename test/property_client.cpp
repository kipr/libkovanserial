#include <kovanserial/kovan_serial.hpp>
#include <kovanserial/usb_serial.hpp>
#include <kovanserial/transport_layer.hpp>

#include <string>

int main(int argc, char *argv[])
{
	if(argc != 3) {
		std::cout << argv[0] << " device property" << std::endl;
		return EXIT_FAILURE; 
	}
	UsbSerial serial(argv[1]);
	if(!serial.makeAvailable()) {
		std::cout << "makeAvailable failed." << std::endl;
		return EXIT_FAILURE;
	}
	
	TransportLayer transport(&serial);
	KovanSerial proto(&transport);
	
	std::string value;
	if(!proto.property(argv[2], value)) {
		std::cout << "Didn't receive " << argv[2] << std::endl;
		return EXIT_FAILURE;
	}
	
	std::cout << argv[2] << " = " << value << std::endl;
	
	return EXIT_SUCCESS;
}