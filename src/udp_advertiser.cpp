#include "kovanserial/udp_advertiser.hpp"
#include "kovanserial/general.hpp"

#include "kovanserial/socket_utils.hpp"

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#define NOMINMAX
#include <stdint.h>
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>

#define AD_PORT 12345
#define AD_GROUP "225.0.0.37"

Advert::Advert()
  : port(0)
{
	memset(serial, 0, 32);
	memset(version, 0, 32);
	memset(device, 0, 32);
	memset(name, 0, 32);
}

Advert::Advert(const char *serial, const char *version,
  const char *device, const char *name, const unsigned short p)
  : port(p)
{
	strncpy(this->serial, serial, 32);
	strncpy(this->version, version, 32);
	strncpy(this->device, device, 32);
	strncpy(this->name, name, 32);
}

bool Advert::operator ==(const Advert &rhs) const
{
  if(memcmp(serial, rhs.serial, 32)) return false;
  if(memcmp(version, rhs.version, 32)) return false;
  if(memcmp(device, rhs.device, 32)) return false;
  if(memcmp(name, rhs.name, 32)) return false;
  return true;
}

bool IncomingAdvert::operator ==(const IncomingAdvert &rhs) const
{
  return ad == rhs.ad && sender.sin_addr.s_addr == rhs.sender.sin_addr.s_addr
    && sender.sin_port == rhs.sender.sin_port;
}

UdpAdvertiser::UdpAdvertiser(bool onlyPulse)
	: m_onlyPulse(onlyPulse),
	m_fd(-1)
{
	memset(&m_group, 0, sizeof(m_group));
	m_group.sin_family = AF_INET;
	m_group.sin_addr.s_addr = inet_addr(AD_GROUP);
	m_group.sin_port = htons(AD_PORT);
	
	setupSocket();
}

UdpAdvertiser::~UdpAdvertiser()
{
}

bool UdpAdvertiser::pulse(const Advert &ad)
{
	int ret = sendto(m_fd, reinterpret_cast<const char *>(&ad),
		sizeof(ad), 0, (sockaddr *)&m_group, sizeof(m_group));
	if(ret < 0) {
#ifdef WIN32
		std::cout << "WSA Error = " << WSAGetLastError() << std::endl;
#endif
	}
	return ret < 0;
}

std::list<IncomingAdvert> UdpAdvertiser::sample(const unsigned long &milli)
{
	std::list<IncomingAdvert> ret;
	long start = msystime();
	do {
		IncomingAdvert ad;
		sockaddr_in sender;
		socklen_t addrlen = sizeof(sender);
		if(recvfrom(m_fd, reinterpret_cast<char *>(&(ad.ad)),
			sizeof(Advert), 0, (sockaddr *)&sender, &addrlen) < 0) {
			if(errno == EAGAIN
#ifdef WIN32
			|| WSAGetLastError() == WSAEWOULDBLOCK	
#endif
			) continue;
			perror("recvfrom");
			return ret;
		}
		ad.sender = sender;
		ret.push_back(ad);
	} while(msystime() - start < milli);
	return ret;
}

void UdpAdvertiser::reset()
{
	setupSocket();
}

void UdpAdvertiser::setupSocket()
{
	if(m_fd >= 0) closeSocket(m_fd);
	
	m_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_fd < 0) {
		perror("socket");
#ifdef WIN32
		std::cout << "Win32 Error: " << WSAGetLastError() << std::endl;
#endif
		return;
	}
	
	uint32_t yes = 1;
	setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR,
		reinterpret_cast<const char *>(&yes), sizeof(yes));
	
	if(!setBlocking(m_fd, false)) {
		perror("fnctl");
		closeSocket(m_fd);
		m_fd = -1;
		return;
	}
	
	if(m_onlyPulse) return;
	
	sockaddr_in groupBind = m_group;
	groupBind.sin_addr.s_addr = htonl(INADDR_ANY);
	
	uint16_t tries = 0;
	if(bind(m_fd, (sockaddr *)&groupBind, sizeof(groupBind)) < 0) {
		perror("bind");
		closeSocket(m_fd);
		m_fd = -1;
		return;
	}
	
	ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(AD_GROUP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if(setsockopt(m_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		reinterpret_cast<const char *>(&mreq), sizeof(mreq)) < 0) {
		perror("setsockopt");
		closeSocket(m_fd);
		m_fd = -1;
		return;
	}
}