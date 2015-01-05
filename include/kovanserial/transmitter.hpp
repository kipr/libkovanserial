#ifndef _TRANSMITTER_HPP_
#define _TRANSMITTER_HPP_

#include "general.hpp"
#include "compat.hpp"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#ifdef _MSC_VER
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

class DLL_EXPORT Transmitter
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
		if(!available()) {
      std::cout << "!available" << std::endl;
      return Transmitter::Error;
    }
		ssize_t ret = readBlock(reinterpret_cast<uint8_t *>(&t), sizeof(T), timeout);
		if(ret < 0) {
      std::cout << "readBlock error" << std::endl;
      perror("readBlock");
      return Transmitter::Error;
    }
		if(ret != sizeof(T)) {
      std::cout << "Timeout" << std::endl;
      return Transmitter::Timeout;
    }
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
