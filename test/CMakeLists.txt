add_definitions(-g -pg)

add_executable(send_file send_file.cpp)
target_link_libraries(send_file kovanserial)

add_executable(server server.cpp)
target_link_libraries(server kovanserial)

add_executable(heartbeat heartbeat.cpp)
target_link_libraries(heartbeat kovanserial)