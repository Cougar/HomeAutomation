cmake_minimum_required(VERSION 2.8)

PROJECT(atom)

add_executable(atom src/vm/plugin/Timer.cpp src/vm/plugin/System.cpp src/vm/Plugin.cpp src/vm/Manager.cpp src/timer/Subscriber.cpp src/can/Module.cpp src/can/Node.cpp src/can/Manager.cpp src/common/IoService.cpp src/net/Subscriber.cpp src/can/Monitor.cpp src/can/Message.cpp src/xml/Node.cpp src/can/Protocol.cpp src/type/Bitset.cpp src/type/Byteset.cpp src/can/Network.cpp src/broker/Subscriber.cpp src/broker/Message.cpp src/broker/Manager.cpp src/config/Manager.cpp src/logging/Logger.cpp src/timer/Timer.cpp src/timer/Manager.cpp src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/main.cpp src/net/Client.cpp src/net/Manager.cpp)


## Link libraries
set(BOOST_LIBS thread system signals program_options filesystem)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(atom ${Boost_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(atom ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(atom v8)

include_directories(src)
add_subdirectory(src)
