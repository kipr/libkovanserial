#ifndef _TRANSMITTER_HPP_
#define _TRANSMITTER_HPP_

#include "general.hpp"
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>

class Transmitter
{
public:
	virtual ~Transmitter();
	
	template<typename T>
	inline bool write(const T &t)
	{
		if(!available()) return false;
		return write(reinterpret_cast<const uint8_t *>(&t), sizeof(T)) >= 0;
	}
	
	template<typename T>
	bool read(T &t, const uint32_t &timeout = 0)
	{
		if(!available()) return false;
		size_t pos = 0;
		long startTime = msystime();
		do {
			long endTime = msystime();
			if(timeout > 0 && endTime - startTime > timeout) return false;
			
			ssize_t ret = read(reinterpret_cast<uint8_t *>(&t) + pos, sizeof(T) - pos);
			if(ret < 0 && errno != EAGAIN) return false;
			if(ret > 0) {
				pos += ret;
				startTime = endTime;
			}
		} while(pos < sizeof(T));
		
		return true;
	}
	
	virtual bool makeAvailable() = 0;
	virtual bool available() const = 0;
	virtual ssize_t write(const uint8_t *data, const size_t &len) = 0;
	virtual ssize_t read(uint8_t *data, const size_t &len) = 0;
};

#endif
