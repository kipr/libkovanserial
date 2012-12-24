#ifndef _SERIAL_COMM_HPP_
#define _SERIAL_COMM_HPP_

#include <cerrno>
#include <stdint.h>
#include <iostream>
#include <sys/time.h>

class SerialComm
{
public:
	SerialComm();
	~SerialComm();
	
	bool open(const char *dev);
	void close();
	bool isOpen() const;
	
	template<typename T>
	inline bool write(const T &t)
	{
		return write(reinterpret_cast<const uint8_t *>(&t), sizeof(T)) >= 0;
	}
	
	template<typename T>
	bool read(T &t, const uint32_t &timeout = 0)
	{
		size_t pos = 0;
		timeval start;
		gettimeofday(&start, NULL);
		long startTime = start.tv_sec * 1000L + start.tv_usec / 1000L;
		do {
			timeval end;
			gettimeofday(&end, NULL);
			long endTime = end.tv_sec * 1000L + end.tv_usec / 1000L;
			if(timeout > 0 && endTime - startTime > timeout) return false;
			
			ssize_t ret = read(reinterpret_cast<uint8_t *>(&t + pos), sizeof(T) - pos);
			if(ret < 0 && errno != EAGAIN) return false;
			if(ret > 0) {
				pos += ret;
				std::cout << "Got " << pos << " of " << sizeof(T) << std::endl;
				startTime = endTime;
			}
		} while(pos < sizeof(T));
		
		return true;
	}
	
	ssize_t write(const uint8_t *data, const size_t &len);
	ssize_t read(uint8_t *data, const size_t &len);

private:
	SerialComm(const SerialComm &rhs);
	void configure();
	
	int m_fd;
};

#endif
