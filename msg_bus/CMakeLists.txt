include_directories(./)
file(GLOB SOURCES "*.cpp")
add_executable(msg_bus ${SOURCES})
