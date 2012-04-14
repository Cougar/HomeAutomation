cmake_minimum_required(VERSION 2.8)

project(atom)

set(CMAKE_CXX_FLAGS "-g -Wall -O0 -Wno-write-strings")


###############################################################################
# Find Subversion client
###############################################################################
find_program(SVN_EXECUTABLE svn DOC "subversion command line client")
macro(GET_SVN_REVISION dir variable)
    execute_process(COMMAND ${SVN_EXECUTABLE} info ${dir} OUTPUT_VARIABLE ${variable} OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "^(.*\n)?Revision: ([^\n]+).*" "\\2" ${variable} "${${variable}}")
endmacro(GET_SVN_REVISION)


###############################################################################
# Find X11
###############################################################################
include(FindX11)


###############################################################################
# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.
###############################################################################

IF (MYSQL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
  /usr/local/include/mysql
  /usr/include/mysql
)

SET(MYSQL_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MYSQL_LIBRARY
  NAMES ${MYSQL_NAMES}
  PATHS /usr/lib /usr/local/lib
  PATH_SUFFIXES mysql
)

IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND TRUE)
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND FALSE)
  SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF (MYSQL_FOUND)
  IF (NOT MYSQL_FIND_QUIETLY)
    MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
  ENDIF (NOT MYSQL_FIND_QUIETLY)
ELSE (MYSQL_FOUND)
  IF (MYSQL_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
  ENDIF (MYSQL_FIND_REQUIRED)
ENDIF (MYSQL_FOUND)

MARK_AS_ADVANCED(
  MYSQL_LIBRARY
  MYSQL_INCLUDE_DIR
)


  
###############################################################################
include_directories(${MYSQL_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)
add_subdirectory(src)

set(CMAKE_BUILD_TYPE Release)

# Find Boost
###############################################################################
set(BOOST_LIBS thread-mt system-mt signals-mt program_options-mt filesystem-mt date_time-mt)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
find_package(Threads REQUIRED)



if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/AtomSvnRev")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/AtomSvnRev" SUBVERSION_REVISION)
else(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/AtomSvnRev")
    GET_SVN_REVISION(${CMAKE_CURRENT_SOURCE_DIR} SUBVERSION_REVISION)        
endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/AtomSvnRev")


set(CMAKE_INSTALL_PREFIX / CACHE INTERNAL "")
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
set(CPACK_PACKAGE_DESCRIPTION "Homeautomation Control Software")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Contains atomd and atomic for controlling your can network. Works in unison with the hardware on project.auml.se.")
set(CPACK_PACKAGE_NAME "atom")
set(CPACK_PACKAGE_CONTACT "Mattias Runge")
set(CPACK_PACKAGE_VENDOR "project.auml.se")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "5")
set(CPACK_PACKAGE_VERSION_PATCH "${SUBVERSION_REVISION}")
set(CPACK_GENERATOR "DEB;")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.6), libgcc1 (>= 1:4.1)")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_PROCESSOR}")

set(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(ATOMD_FILES src/common/log.cpp src/common/common.cpp src/control/Code.cpp src/vm/plugin/Storage.cpp src/storage/Store.cpp src/storage/Manager.cpp src/vm/plugin/Socket.cpp src/vm/plugin/Console.cpp src/vm/plugin/Module.cpp src/vm/plugin/Node.cpp src/vm/plugin/Timer.cpp src/vm/plugin/System.cpp src/vm/Plugin.cpp src/vm/Manager.cpp src/timer/Subscriber.cpp src/control/Module.cpp src/control/Node.cpp src/control/Manager.cpp src/common/IoService.cpp src/net/Subscriber.cpp src/can/Monitor.cpp src/can/CanDaemon.cpp src/can/Message.cpp src/xml/Node.cpp src/can/Protocol.cpp src/common/Bitset.cpp src/common/Byteset.cpp src/can/Network.cpp src/broker/Subscriber.cpp src/broker/Message.cpp src/broker/Manager.cpp src/config/Manager.cpp src/logging/Logger.cpp src/timer/Timer.cpp src/timer/Manager.cpp src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/net/Client.cpp src/net/Manager.cpp src/mbb/Manager.cpp src/atomd.cpp)
set(ATOMD_LIBS ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} v8)

set(ATOMIC_FILES src/common/common.cpp src/atomic.cpp src/logging/Logger.cpp src/common/IoService.cpp src/net/Subscriber.cpp src/common/Byteset.cpp src/net/SerialClient.cpp src/net/UdpClient.cpp src/net/TcpClient.cpp src/net/Client.cpp src/net/Manager.cpp)
set(ATOMIC_LIBS ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} readline)

if(${X11_FOUND})
    message(STATUS "Building with Xorg support")
    set(USE_PLUGIN_XORG ON)
    set(ATOMD_FILES ${ATOMD_FILES} src/vm/plugin/Xorg.cpp)
    set(ATOMD_LIBS ${ATOMD_LIBS} ${X11_LIBRARIES})
endif(${X11_FOUND})

if(${MYSQL_FOUND})
    message(STATUS "Building with MySql support")
    set(USE_PLUGIN_MYSQL ON)
    set(ATOMD_FILES ${ATOMD_FILES} src/vm/plugin/MySql.cpp)
    set(ATOMD_LIBS ${ATOMD_LIBS} ${MYSQL_LIBRARIES})
endif(${MYSQL_FOUND})

add_executable(atomd ${ATOMD_FILES})
target_link_libraries(atomd ${ATOMD_LIBS})

add_executable(atomic ${ATOMIC_FILES})
target_link_libraries(atomic ${ATOMIC_LIBS})

#NEEDED               libpthread.so.0
#NEEDED               libboost_thread.so.1.42.0
#NEEDED               libboost_system.so.1.42.0
#NEEDED               libboost_signals.so.1.42.0
#NEEDED               libboost_program_options.so.1.42.0
#NEEDED               libboost_filesystem.so.1.42.0
#NEEDED               libboost_date_time.so.1.42.0
#NEEDED               libv8.so.2.2.18
#NEEDED               libX11.so.6
#NEEDED               libstdc++.so.6
#NEEDED               libm.so.6
#NEEDED               libgcc_s.so.1
#NEEDED               libc.so.6
#NEEDED               libreadline.so.6
# objdump -p build/atomic | grep NEEDED
# apt-file search libpthread | grep /usr/lib

# TODO: Fix dependencies

message(STATUS "SVN revision is ${SUBVERSION_REVISION}")

install(TARGETS atomd RUNTIME DESTINATION usr/bin)
install(TARGETS atomic RUNTIME DESTINATION usr/bin)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/script" DESTINATION usr/share/atom FILES_MATCHING PATTERN "*.js" PATTERN ".svn" EXCLUDE)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/uscript" DESTINATION etc/atom DIRECTORY_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE FILES_MATCHING PATTERN "*" PATTERN ".svn" EXCLUDE)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/storage" DESTINATION etc/atom DIRECTORY_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE FILES_MATCHING PATTERN "*" PATTERN ".svn" EXCLUDE)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/atom.conf.template" DESTINATION etc/atom)
# install upstart file
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/debian/atom.conf" DESTINATION etc/init)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../Configuration/data.xml")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../../Configuration/data.xml" DESTINATION usr/share/atom/protocol)
else(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../Configuration/data.xml")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Configuration/data.xml" DESTINATION usr/share/atom/protocol)
endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../Configuration/data.xml")

include(CPack)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
