cmake_minimum_required(VERSION 2.8)

PROJECT(atom)

add_executable(atom src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/main.cpp src/net/Client.cpp src/net/Manager.cpp)


## Link libraries
set(BOOST_LIBS thread system signals)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(atom ${Boost_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(atom ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(atom v8)

include_directories(src)
add_subdirectory(src)
