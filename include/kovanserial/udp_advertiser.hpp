#ifndef _UDP_ADVERTISER_HPP_
#define _UDP_ADVERTISER_HPP_

#include <string>
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>

struct Advert
{
	Advert();
	Advert(const char *serial, const char *version,
		const char *device, const char *name);
	
	char serial[32];
	char version[32];
	char device[32];
	char name[32];
};

struct IncomingAdvert
{
	Advert ad;
	sockaddr_in sender;
};

class UdpAdvertiser
{
public:
	UdpAdvertiser();
	~UdpAdvertiser();
	
	bool pulse(const Advert &ad);
	std::list<IncomingAdvert> sample(const unsigned long &milli);
	
	void reset();
private:
	void setupSocket();
	
	int m_fd;
	sockaddr_in m_group;
};

#endif
