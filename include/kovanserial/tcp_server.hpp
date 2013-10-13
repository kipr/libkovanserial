#ifndef _TCP_SERVER_HPP_
#define _TCP_SERVER_HPP_

#include "tcp.hpp"
#include "compat.hpp"

class DLL_EXPORT TcpServer : public Tcp
{
public:
	TcpServer();
	~TcpServer();
	
	bool bind(const unsigned short port);
	bool listen(const int &backLog);
	bool accept(uint64_t timeout);
	
	enum ConnectionRestriction {
		None,
		OnlyLocal
	};
	
	void setConnectionRestriction(const ConnectionRestriction restriction);
	ConnectionRestriction connectionRestriction() const;
	
	virtual bool makeAvailable();
	virtual void endSession();
	virtual bool isReliable() const;
	
private:
	int m_ourFd;
	ConnectionRestriction m_restriction;
};

#endif
