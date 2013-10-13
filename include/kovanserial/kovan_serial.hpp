#ifndef _KOVAN_SERIAL_HPP_
#define _KOVAN_SERIAL_HPP_

#include <iostream>
#include <string>
#include <list>

#include <kovanserial/transport_layer.hpp>
#include <kovanserial/command_types.hpp>

#include "compat.hpp"

#define KOVAN_SERIAL_PORT 8374
#define KOVAN_SERIAL_PROTOCOL_VERSION "2"

class DLL_EXPORT KovanSerial
{
public:
	KovanSerial(TransportLayer *transport);
	~KovanSerial();
	
	bool knockKnock(uint32_t timeout = 0);
	bool whosThere();
	
	bool protocolVersion(std::string &version);
	bool sendProtocolVersion();
	
	bool setPassword(const std::string &password);
	void setNoPassword();
	
	bool isPassworded() const;
	const uint8_t *passwordMd5() const;
	const uint8_t *passwordSha1() const;
	
	bool authenticationInfo(bool &authNecessary);
	bool sendAuthenticationInfo(const bool authNecessary);
	bool requestAuthentication(bool &success, uint8_t *const sessionKey);
	bool confirmAuthentication(const bool success);
	
	bool sendProperty(const std::string &name, const std::string &value);
	bool property(const std::string &name, std::string &value);
	
	bool sendPropertyList(const std::list<std::string> &names);
	bool listProperties(std::list<std::string> &names);
	
	bool sendFile(const std::string &dest, const std::string &metadata, std::istream *in);
	bool sendFile(const std::string &dest, const std::string &metadata, const unsigned char *data,
		const size_t size);
	bool confirmFile(const bool &good);
	bool recvFile(const size_t &size, std::ostream *out, const uint32_t &timeout = 0);
	
	bool sendFileAction(const std::string &action, const std::string &file);
	bool confirmFileAction(const bool &good);
	
	bool sendFileActionProgress(const bool &finished, const double &progress);
	bool recvFileActionProgress(bool &finished, double &progress, const uint32_t &timeout = 0);
	
	TransportLayer::Return next(Packet &p, const uint32_t &timeout = 0);
	
	void hangup();
	void clearSession();

	
private:
	bool error(const TransportLayer::Return ret);
	
	TransportLayer *m_transport;
	
	bool m_passworded;
	uint8_t m_md5[16];
	uint8_t m_sha1[20];
};


#endif
