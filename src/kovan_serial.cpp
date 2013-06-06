#include "kovanserial/kovan_serial.hpp"
#include "kovanserial/transport_layer.hpp"
#include "kovanserial/md5.hpp"
#include "kovanserial/sha1.hpp"
#include "kovanserial/randomize.hpp"
#include "kovanserial/xor.hpp"

#include <string.h>

KovanSerial::KovanSerial(TransportLayer *transport)
	: m_transport(transport)
{
	setNoPassword();
	randomize::seedOnce();
}

KovanSerial::~KovanSerial()
{
	
}

bool KovanSerial::knockKnock(uint32_t timeout)
{
	char dummy = 0;
	if(error(m_transport->send(Packet(Command::KnockKnock, dummy)))) return false;
	Packet p;
	return !error(m_transport->recv(p, timeout)) && p.type == Command::WhosThere;
}

bool KovanSerial::whosThere()
{
	char dummy = 0;
	return !error(m_transport->send(Packet(Command::WhosThere, dummy)));
}

bool KovanSerial::setPassword(const std::string &password)
{
	memset(m_md5, 0, sizeof(m_md5));
	memset(m_sha1, 0, sizeof(m_sha1));
	m_passworded = !password.empty();
	if(!m_passworded) return false;
	
	MD5 md5(password);
	memcpy(m_md5, md5.digest(), sizeof(m_md5));
	
	SHA1 sha1;
	sha1.update((const uint8_t *)password.c_str(), password.size());
	sha1.final(m_sha1);
	
	return true;
}

void KovanSerial::setNoPassword()
{
	setPassword(std::string());
}

bool KovanSerial::isPassworded() const
{
	return m_passworded;
}

const uint8_t *KovanSerial::passwordMd5() const
{
	return m_md5;
}

const uint8_t *KovanSerial::passwordSha1() const
{
	return m_sha1;
}

bool KovanSerial::authenticationInfo(bool &authNecessary)
{
	clearSession();
	
	if(error(m_transport->send(Packet(Command::RequestAuthenticationInfo), true))) {
		std::cout << "Failed to send auth info request" << std::endl;
		return false;
	}
	
	Packet p;
	if(error(m_transport->recv(p, 2000)) || p.type != Command::AuthenticationInfo) {
		std::cout << "Did not receive authentication info" << std::endl;
		return false;
	}
	
	Command::AuthenticationInfoData data;
	p.as(data);
	
	authNecessary = data.authNecessary;
	
	return true;
}

bool KovanSerial::sendAuthenticationInfo(const bool authNecessary)
{
	clearSession();
	
	Command::AuthenticationInfoData data;
	data.authNecessary = authNecessary;
	
	if(error(m_transport->send(Packet(Command::AuthenticationInfo, data), true))) {
		std::cout << "Failed to send auth info" << std::endl;
		return false;
	}
	
	return true;
}

bool KovanSerial::requestAuthentication(bool &success, uint8_t *const sessionKey)
{
	if(!m_passworded) return false;
	
	clearSession();

	Command::RequestAuthenticationData req;
	memcpy(req.password, m_md5, 16);
	
	if(error(m_transport->send(Packet(Command::RequestAuthentication, req), true))) {
		std::cout << "Failed to send auth info request" << std::endl;
		return false;
	}
	
	Packet p;
	if(error(m_transport->recv(p, 200)) || p.type != Command::ConfirmAuthentication) {
		std::cout << "Did not receive authentication info" << std::endl;
		return false;
	}
	
	Command::ConfirmAuthenticationData data;
	p.as(data);
	
	success = data.success;
	if(!success) return true;
	
	xor_crypt::crypt(data.scrambledSessionKey, sessionKey, KOVAN_SERIAL_SESSION_KEY_SIZE,
		m_md5, sizeof(m_md5));
		
	m_transport->setKey(sessionKey, KOVAN_SERIAL_SESSION_KEY_SIZE);
	
	return true;
}

bool KovanSerial::confirmAuthentication(const bool success)
{
	clearSession();
	
	Command::ConfirmAuthenticationData data;
	data.success = success;
	uint8_t sessionKey[KOVAN_SERIAL_SESSION_KEY_SIZE];
	memset(data.scrambledSessionKey, 0, KOVAN_SERIAL_SESSION_KEY_SIZE);
	if(success) {
		randomize::fill(sessionKey, KOVAN_SERIAL_SESSION_KEY_SIZE);
		xor_crypt::crypt(sessionKey, data.scrambledSessionKey,
			KOVAN_SERIAL_SESSION_KEY_SIZE, m_md5, sizeof(m_md5));
	}
	if(error(m_transport->send(Packet(Command::ConfirmAuthentication, data), true))) {
		std::cout << "Failed to send auth confirm" << std::endl;
		return false;
	}
	
	m_transport->setKey(sessionKey, KOVAN_SERIAL_SESSION_KEY_SIZE);
	
	return true;
}

bool KovanSerial::sendProperty(const std::string &name, const std::string &value)
{
	Command::PropertyValuePairData data;
	strncpy(data.name, name.c_str(), 8);
	strncpy(data.value, value.c_str(), 128);
	if(error(m_transport->send(Packet(Command::Property, data)))) {
		std::cout << "Failed to send property" << std::endl;
		return false;
	}
	return true;
}

bool KovanSerial::property(const std::string &name, std::string &value)
{
	Command::PropertyData data;
	strncpy(data.name, name.c_str(), 8);
	if(error(m_transport->send(Packet(Command::Property, data)))) {
		std::cout << "Failed to send property request" << std::endl;
		return false;
	}
	
	Packet p;
	if(error(m_transport->recv(p, 1000)) || p.type != Command::Property) {
		std::cout << "Did not receive property value pair" << std::endl;
		return false;
	}
	
	Command::PropertyValuePairData ret;
	p.as(ret);
	ret.value[127] = 0; // Make sure we're null termed
	value = ret.value;
	return true;
}

bool KovanSerial::sendPropertyList(const std::list<std::string> &names)
{
	Command::PropertyListData data;
	data.size = names.size();
	
	std::list<std::string>::const_iterator it = names.begin();
	size_t i = 0;
	for(; it != names.end() && i < 50; ++it, ++i) {
		Command::PropertyData entry;
		strncpy(entry.name, (*it).c_str(), 8);
		data.names[i] = entry;
	}
	
	if(error(m_transport->send(Packet(Command::Property, data)))) {
		std::cout << "Failed to send property" << std::endl;
		return false;
	}
	return true;
}

bool KovanSerial::listProperties(std::list<std::string> &names)
{
	return false;
}

bool KovanSerial::sendFile(const std::string &dest, const std::string &metadata, std::istream *in)
{
	Command::FileHeaderData header;
	strncpy(header.dest, dest.c_str(), 256);
	strncpy(header.metadata, metadata.c_str(), 200);
	in->seekg(0, std::ios::end);
	header.size = in->tellg();
	in->seekg(0, std::ios::beg);
	
	std::cout << "Sending file header." << std::endl;
	if(error(m_transport->send(Packet(Command::FileHeader, header)))) return false;
	
	std::cout << "Waiting on confirm..." << std::endl;
	Packet confirm;
	if(error(m_transport->recv(confirm, 15000)) || confirm.type != Command::FileConfirm) {
		std::cout << "Didn't receive confirm. Aborting." << std::endl;
		return false;
	}
	std::cout << "Got confirm packet." << std::endl;
	
	bool good = false;
	confirm.as(good);
	if(!good) {
		std::cout << "Other side rejected our transfer." << std::endl;
		return false;
	}
	std::cout << "Sening file..." << std::endl;
	uint8_t buffer[TRANSPORT_MAX_DATA_SIZE];
	size_t i = 0;
	while(!in->eof() && i < header.size) {
		in->read(reinterpret_cast<char *>(buffer), TRANSPORT_MAX_DATA_SIZE);
		i += in->gcount();
		if(error(m_transport->send(Packet(Command::File,
			buffer, TRANSPORT_MAX_DATA_SIZE)))) {
			std::cout << "sending file packet failed" << std::endl;
			return false;
		}
		std::cout << "Sent " << i << " of " << header.size << std::endl;
	}
	
	std::cout << "Finished writing entire file" << std::endl;
	
	return true;
}

bool KovanSerial::confirmFile(const bool &good)
{
	return !error(m_transport->send(Packet(Command::FileConfirm, good)));
}

bool KovanSerial::recvFile(const size_t &size, std::ostream *out, const uint32_t &timeout)
{
	size_t i = 0;
	
	Packet p;
	while(i < size) {
		if(error(m_transport->recv(p, timeout))) {
			std::cout << "recvFile timed out" << std::endl;
			return false;
		}
		
		if(p.type != Command::File) {
			std::cerr << "Non-file type in file stream." << std::endl;
			return false;
		}
		
		out->write(reinterpret_cast<const char *>(p.data),
			std::min(TRANSPORT_MAX_DATA_SIZE, size - i));
		i += TRANSPORT_MAX_DATA_SIZE;
	}
	
	return true;
}

bool KovanSerial::sendFileAction(const std::string &action, const std::string &file)
{
	Command::FileActionData data;
	strncpy(data.action, action.c_str(), 8);
	strncpy(data.dest, file.c_str(), 500);
	
	if(error(m_transport->send(Packet(Command::FileAction, data)))) {
		std::cout << "Failed to send file action" << std::endl;
		return false;
	}
	
	Packet confirm;
	if(error(m_transport->recv(confirm, 10000)) || confirm.type != Command::FileActionConfirm) {
		std::cout << "Didn't receive confirm. Aborting." << std::endl;
		return false;
	}
	
	bool good = false;
	confirm.as(good);
	if(!good) {
		std::cout << "Other side rejected our action." << std::endl;
		return false;
	}
	
	return true;
}

bool KovanSerial::confirmFileAction(const bool &good)
{
	return !error(m_transport->send(Packet(Command::FileActionConfirm,
		reinterpret_cast<const uint8_t *>(&good), sizeof(bool))));
}

bool KovanSerial::sendFileActionProgress(const bool &finished, const double &progress)
{
	Command::FileActionProgressData data;
	data.finished = finished;
	data.progress = progress;
	return !error(m_transport->send(Packet(Command::FileActionProgress, data)));
}

bool KovanSerial::recvFileActionProgress(bool &finished, double &progress, const uint32_t &timeout)
{
	Packet p;
	if(error(m_transport->recv(p, timeout)) || p.type != Command::FileActionProgress) {
		std::cout << "Couldn't recv file action progress" << std::endl;
		return false;
	}
	
	Command::FileActionProgressData data;
	p.as(data);
	
	finished = data.finished;
	progress = data.progress;
	
	return true;
}

TransportLayer::Return  KovanSerial::next(Packet &p, const uint32_t &timeout)
{
	return m_transport->recv(p, timeout);
}

void KovanSerial::hangup()
{
	m_transport->send(Packet(Command::Hangup), true);
	clearSession();
}

void KovanSerial::clearSession()
{
	m_transport->setKey(0, 0);
}

bool KovanSerial::error(const TransportLayer::Return ret)
{
	if(ret == TransportLayer::UntrustedSuccess && !m_transport->key()) return false;
	return ret != TransportLayer::Success;
}
