#ifndef _TRANSMITTER_HPP_
#define _TRANSMITTER_HPP_

#include "general.hpp"
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>

class Transmitter
{
public:
	enum Return {
		Success = 0,
		Error,
		Timeout
	};
	
	virtual ~Transmitter();
	
	template<typename T>
	inline Transmitter::Return write(const T &t)
	{
		if(!available()) return Transmitter::Error;
		return write(reinterpret_cast<const uint8_t *>(&t), sizeof(T)) >= 0 ? Transmitter::Success : Transmitter::Error;
	}
	
	template<typename T>
	inline Transmitter::Return read(T &t, const uint32_t timeout = 0)
	{
		if(!available()) return Transmitter::Error;
		ssize_t ret = readBlock(reinterpret_cast<uint8_t *>(&t), sizeof(T), timeout);
		if(ret < 0) return Transmitter::Error;
		if(ret != sizeof(T)) return Transmitter::Timeout;
		return Transmitter::Success;
	}
	
	virtual bool makeAvailable() = 0;
	virtual void endSession() = 0;
	virtual bool isReliable() const = 0;
	virtual bool available() const = 0;
	
	virtual ssize_t readBlock(uint8_t *data, const size_t len, const uint32_t timeout = 0) = 0;
	
	virtual ssize_t write(const uint8_t *data, const size_t len) = 0;
	virtual ssize_t read(uint8_t *data, const size_t len) = 0;
};

#endif
