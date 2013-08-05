project(libkovanserial)

cmake_minimum_required(VERSION 2.8.0)

include_directories(include)

file(GLOB INCLUDES include/kovanserial/*.hpp include/kovanserial/*.h)
file(GLOB SOURCES ${libkovanserial_SOURCE_DIR}/src/*.c ${libkovanserial_SOURCE_DIR}/src/*.cpp)

add_definitions(-g -pg)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

if(UNIX)
	list(REMOVE_ITEM SOURCES ${libkovanserial_SOURCE_DIR}/src/usb_serial_win.cpp)
elseif(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
	set(CMAKE_SHARED_LINKER_FLAGS "-pg ${CMAKE_SHARED_LINKER_FLAGS}")
	list(REMOVE_ITEM SOURCES ${libkovanserial_SOURCE_DIR}/src/usb_serial_unix.cpp)
endif()

add_library(kovanserial SHARED ${SOURCES})

if(WIN32)
target_link_libraries(kovanserial ws2_32)
install(FILES ${INCLUDES} DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/include/kovanserial)
install(TARGETS kovanserial
	ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/lib
	RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/../prefix/lib)
else()
install(FILES ${INCLUDES} DESTINATION include/kovanserial)
install(TARGETS kovanserial LIBRARY DESTINATION lib)
endif()
