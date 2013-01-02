#ifndef _TCP_SERVER_HPP_
#define _TCP_SERVER_HPP_

#include "tcp.hpp"

class TcpServer : public Tcp
{
public:
	TcpServer();
	~TcpServer();
	
	bool bind(const char *port);
	bool listen(const int &backLog);
	bool accept(uint64_t timeout);
	
	virtual bool makeAvailable();
	virtual void endSession();
	virtual bool isReliable() const;
	
private:
	int m_ourFd;
};

#endif
