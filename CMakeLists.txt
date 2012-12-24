project(libkovanserial)

cmake_minimum_required(VERSION 2.6)

include_directories(include)

file(GLOB INCLUDES include/kovanserial/*.hpp include/kovanserial/*.h)
file(GLOB SOURCES src/transport_layer.cpp src/kovan_serial.cpp src/serial_comm.cpp src/crc.c)

add_definitions(-g -pg)

if(UNIX)
	set(SOURCES ${SOURCES} src/serial_comm_unix.cpp)
elseif(WIN32)
	message(FATAL_ERROR "Windows not supported")
endif(UNIX)

add_library(kovanserial SHARED ${SOURCES})

install(FILES ${INCLUDES} DESTINATION include/kovanserial)
install(TARGETS kovanserial LIBRARY DESTINATION lib)

add_subdirectory(test)