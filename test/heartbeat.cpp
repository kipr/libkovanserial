#include <kovanserial/udp_advertiser.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
	UdpAdvertiser advertiser(true);
	Advert myself("???", "1.0", "computer", "beta");
	for(;;) {
		std::cout << "pulse" << std::endl;
		advertiser.pulse(myself);
		std::list<IncomingAdvert> adverts = advertiser.sample(1000);
		std::list<IncomingAdvert>::iterator it = adverts.begin();
		for(; it != adverts.end(); ++it) {
			std::cout << "Name " << (*it).ad.name << std::endl;
		}
	}
}