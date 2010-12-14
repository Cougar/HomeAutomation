cmake_minimum_required(VERSION 2.8)

PROJECT(atom)

add_executable(atomic src/atomic.cpp src/logging/Logger.cpp src/common/IoService.cpp src/net/Subscriber.cpp src/common/Byteset.cpp src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/net/Client.cpp src/net/Manager.cpp)
add_executable(atomd src/control/Code.cpp src/vm/plugin/Storage.cpp src/storage/Store.cpp src/storage/Manager.cpp src/vm/plugin/Console.cpp src/vm/plugin/Module.cpp src/vm/plugin/Timer.cpp src/vm/plugin/System.cpp src/vm/Plugin.cpp src/vm/Manager.cpp src/timer/Subscriber.cpp src/control/Module.cpp src/control/Node.cpp src/control/Manager.cpp src/common/IoService.cpp src/net/Subscriber.cpp src/can/Monitor.cpp src/can/Message.cpp src/xml/Node.cpp src/can/Protocol.cpp src/common/Bitset.cpp src/common/Byteset.cpp src/can/Network.cpp src/broker/Subscriber.cpp src/broker/Message.cpp src/broker/Manager.cpp src/config/Manager.cpp src/logging/Logger.cpp src/timer/Timer.cpp src/timer/Manager.cpp src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/net/Client.cpp src/net/Manager.cpp src/atomd.cpp)


set(BOOST_LIBS thread system signals program_options filesystem date_time)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(atomic ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(atomic ${Boost_LIBRARIES})
target_link_libraries(atomic readline)
target_link_libraries(atomd ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(atomd ${Boost_LIBRARIES})
target_link_libraries(atomd v8)


include_directories(src)
add_subdirectory(src)

