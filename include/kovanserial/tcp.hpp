#ifndef _TCP_HPP_
#define _TCP_HPP_

#include "transmitter.hpp"

class Tcp : public Transmitter
{
public:
	Tcp();
	~Tcp();
	
	bool available() const;
	
	ssize_t readBlock(uint8_t *data, const size_t len, const uint32_t timeout = 0);
	
	ssize_t write(const uint8_t *data, const size_t len);
	ssize_t read(uint8_t *data, const size_t len);
	
protected:
	void closeFd();
	void setFd(const int &fd);
	const int &fd() const;

private:
	int m_fd;
};

#endif
