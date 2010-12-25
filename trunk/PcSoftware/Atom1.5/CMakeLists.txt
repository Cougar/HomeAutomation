cmake_minimum_required(VERSION 2.8)

PROJECT(atom)

FIND_PROGRAM(SVN_EXECUTABLE svn DOC "subversion command line client")

MACRO(Subversion_GET_REVISION dir variable)
    EXECUTE_PROCESS(COMMAND ${SVN_EXECUTABLE} info ${dir} OUTPUT_VARIABLE ${variable} OUTPUT_STRIP_TRAILING_WHITESPACE)
    STRING(REGEX REPLACE "^(.*\n)?Revision: ([^\n]+).*" "\\2" ${variable} "${${variable}}")
ENDMACRO(Subversion_GET_REVISION)

Subversion_GET_REVISION(${CMAKE_CURRENT_SOURCE_DIR} SUBVERSION_REVISION)
MESSAGE(STATUS "Revision is ${SUBVERSION_REVISION}")

add_executable(atomic src/common/common.cpp src/atomic.cpp src/logging/Logger.cpp src/common/IoService.cpp src/net/Subscriber.cpp src/common/Byteset.cpp src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/net/Client.cpp src/net/Manager.cpp)
add_executable(atomd src/common/common.cpp src/control/Code.cpp src/vm/plugin/Xorg.cpp src/vm/plugin/Storage.cpp src/storage/Store.cpp src/storage/Manager.cpp src/vm/plugin/Socket.cpp src/vm/plugin/Console.cpp src/vm/plugin/Module.cpp src/vm/plugin/Timer.cpp src/vm/plugin/System.cpp src/vm/Plugin.cpp src/vm/Manager.cpp src/timer/Subscriber.cpp src/control/Module.cpp src/control/Node.cpp src/control/Manager.cpp src/common/IoService.cpp src/net/Subscriber.cpp src/can/Monitor.cpp src/can/Message.cpp src/xml/Node.cpp src/can/Protocol.cpp src/common/Bitset.cpp src/common/Byteset.cpp src/can/Network.cpp src/broker/Subscriber.cpp src/broker/Message.cpp src/broker/Manager.cpp src/config/Manager.cpp src/logging/Logger.cpp src/timer/Timer.cpp src/timer/Manager.cpp src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/net/Client.cpp src/net/Manager.cpp src/atomd.cpp)


set(BOOST_LIBS thread system signals program_options filesystem date_time)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(atomic ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(atomic ${Boost_LIBRARIES})
target_link_libraries(atomic readline)
target_link_libraries(atomd ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(atomd ${Boost_LIBRARIES})
target_link_libraries(atomd v8)

IF(${USE_PLUGIN_XORG})
MESSAGE(STATUS "Building with Xorg support")
target_link_libraries(atomd X11)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-iostreams-dev, libboost-date-time-dev, libboost-filesystem-dev, libboost-program-options-dev, libboost-signals-dev, libboost-thread-dev, libreadline-dev, libv8-dev, libx11-dev (>= 2:1.3)")
ENDIF(${USE_PLUGIN_XORG})

IF(NOT ${USE_PLUGIN_XORG})
MESSAGE(STATUS "Building without Xorg support")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.6), libgcc1 (>= 1:4.1), libv8-dev (>= 2.2.18), libboost-iostreams-dev, libboost-date-time-dev, libboost-filesystem-dev, libboost-program-options-dev, libboost-signals-dev, libboost-thread-dev, libreadline-dev, libv8-dev")
ENDIF(NOT ${USE_PLUGIN_XORG})


include_directories(src)
include_directories(build)
add_subdirectory(src)

install(TARGETS atomd RUNTIME DESTINATION bin)
install(TARGETS atomic RUNTIME DESTINATION bin)

FILE(GLOB script_common_files "${CMAKE_CURRENT_SOURCE_DIR}/script/common/*.js")
INSTALL(FILES ${script_common_files} DESTINATION share/atom/script/common)

FILE(GLOB script_interface_files "${CMAKE_CURRENT_SOURCE_DIR}/script/interface/*.js")
INSTALL(FILES ${script_interface_files} DESTINATION share/atom/script/interface)

FILE(GLOB script_plugin_files "${CMAKE_CURRENT_SOURCE_DIR}/script/plugin/*.js")
INSTALL(FILES ${script_plugin_files} DESTINATION share/atom/script/plugin)

FILE(GLOB script_user_files "${CMAKE_CURRENT_SOURCE_DIR}/script/user/*.js")
INSTALL(FILES ${script_user_files} DESTINATION share/atom/script/user)

INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/storage/Remote_Yamaha_RAV34" DESTINATION share/atom/storage)
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/storage/Remote_LG_MKJ42519615" DESTINATION share/atom/storage)
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/storage/Remote_Leadtek_PVR2000" DESTINATION share/atom/storage)
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/storage/RemoteList" DESTINATION share/atom/storage)
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/storage/Xorg_Keymap" DESTINATION share/atom/storage)

INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/atom.conf.template" DESTINATION share/atom)

set(CPACK_PACKAGE_DESCRIPTION "Homeautomation Control Software")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Contains atomd and atomic for controlling your can network. Works in unison with the hardware on project.auml.se.")
set(CPACK_PACKAGE_NAME "Atom")

set(CPACK_PACKAGE_CONTACT "Mattias Runge")
set(CPACK_PACKAGE_VENDOR "project.auml.se")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "5")
set(CPACK_PACKAGE_VERSION_PATCH "${SUBVERSION_REVISION}")
set(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_GENERATOR "DEB;")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CMAKE_SYSTEM_PROCESSOR}")

include(CPack)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# To enable the Xorg plugin
# cmake -DUSE_PLUGIN_XORG=true ../
