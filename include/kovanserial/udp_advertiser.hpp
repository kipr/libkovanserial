#ifndef _UDP_ADVERTISER_HPP_
#define _UDP_ADVERTISER_HPP_

#include <string>
#include <list>
#include "compat.hpp"
#ifdef WIN32
#define _WIN32_WINNT 0x0501
#define NOMINMAX
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

struct DLL_EXPORT Advert
{
	Advert();
	Advert(const char *serial, const char *version,
		const char *device, const char *name, const unsigned short port);
	
	char serial[32];
	char version[32];
	char device[32];
	char name[32];
  unsigned short port;
  
  bool operator ==(const Advert &rhs) const;
};

struct DLL_EXPORT IncomingAdvert
{
	Advert ad;
	sockaddr_in sender;
  
  bool operator ==(const IncomingAdvert &rhs) const;
};

class DLL_EXPORT UdpAdvertiser
{
public:
	UdpAdvertiser(bool onlyPulse);
	~UdpAdvertiser();
	
	bool pulse(const Advert &ad);
	std::list<IncomingAdvert> sample(const unsigned long &milli);
	
	void reset();
private:
	void setupSocket();
	
	bool m_onlyPulse;
	int m_fd;
	sockaddr_in m_group;
};

#endif
