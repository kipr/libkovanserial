if(build_tests)

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

add_executable(auth auth.cpp)
target_link_libraries(auth kovanserial)

enable_testing()

add_executable(sha1_test sha1_test.cpp)
target_link_libraries(sha1_test kovanserial boost_unit_test_framework-mt boost_test_exec_monitor-mt)
add_test(sha1 sha1_test)

endif()