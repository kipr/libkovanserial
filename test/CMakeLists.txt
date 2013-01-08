add_executable(send_file send_file.cpp)
target_link_libraries(send_file kovanserial)

add_executable(server server.cpp)
target_link_libraries(server kovanserial)

add_executable(heartbeat heartbeat.cpp)
target_link_libraries(heartbeat kovanserial)

add_executable(send_action send_action.cpp)
target_link_libraries(send_action kovanserial)

add_executable(recv_action recv_action.cpp)
target_link_libraries(recv_action kovanserial)

add_executable(property_server property_server.cpp)
target_link_libraries(property_server kovanserial)

add_executable(property_client property_client.cpp)
target_link_libraries(property_client kovanserial)

add_executable(screenshot screenshot.cpp)
target_link_libraries(screenshot kovanserial)
