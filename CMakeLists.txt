project(libkovanserial)

cmake_minimum_required(VERSION 2.6)

include_directories(include)

file(GLOB INCLUDES include/kovanserial/*.hpp include/kovanserial/*.h)
file(GLOB SOURCES
	src/transport_layer.cpp
	src/kovan_serial.cpp
	src/general.cpp
	src/tcp.cpp
	src/tcp_server.cpp
	src/transmitter.cpp
	src/socket_utils.cpp
	src/kovan_server.cpp
	src/udp_advertiser.cpp
	src/tcp_serial.cpp
	src/crc.c)

add_definitions(-g -pg)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

if(UNIX)
	set(SOURCES ${SOURCES} src/usb_serial_unix.cpp)
elseif(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
	set(CMAKE_SHARED_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
	set(SOURCES ${SOURCES} src/usb_serial_win.cpp)
#	message(FATAL_ERROR "Windows not supported")
endif(UNIX)

add_library(kovanserial SHARED ${SOURCES})

IF(WIN32)
target_link_libraries(kovanserial ws2_32)
install(FILES ${INCLUDES} DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/include/kovanserial)
INSTALL(TARGETS kovanserial
	ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/lib
	RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/lib)
ELSE(WIN32)
install(FILES ${INCLUDES} DESTINATION include/kovanserial)
install(TARGETS kovanserial LIBRARY DESTINATION lib)
ENDIF(WIN32)

add_subdirectory(test)