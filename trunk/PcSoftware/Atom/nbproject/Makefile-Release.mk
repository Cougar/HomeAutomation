#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran

# Macros
PLATFORM=GNU-Linux-x86

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Release/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/Socket/server.o \
	${OBJECTDIR}/src/Logger/logger.o \
	${OBJECTDIR}/src/CanNet/canmessage.o \
	${OBJECTDIR}/src/VM/intervalthread.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/CanNet/cannetmanager.o \
	${OBJECTDIR}/src/VM/virtualmachine.o \
	${OBJECTDIR}/src/VM/commandthread.o \
	${OBJECTDIR}/src/singleton.o \
	${OBJECTDIR}/src/VM/socketthread.o \
	${OBJECTDIR}/src/CanNet/canidtranslator.o \
	${OBJECTDIR}/src/CanNet/candebug.o \
	${OBJECTDIR}/src/Xml/xmlnode.o \
	${OBJECTDIR}/src/Tools/tools.o \
	${OBJECTDIR}/src/Settings/settings.o \
	${OBJECTDIR}/src/Socket/asyncsocket.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread src/v8/libv8.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk atom

atom: src/v8/libv8.a

atom: ${OBJECTFILES}
	${LINK.cc} -m32 -o atom ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/Socket/server.o: src/Socket/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Socket
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Socket/server.o src/Socket/server.cpp

${OBJECTDIR}/src/Logger/logger.o: src/Logger/logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Logger
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Logger/logger.o src/Logger/logger.cpp

${OBJECTDIR}/src/CanNet/canmessage.o: src/CanNet/canmessage.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CanNet/canmessage.o src/CanNet/canmessage.cpp

${OBJECTDIR}/src/VM/intervalthread.o: src/VM/intervalthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/VM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/VM/intervalthread.o src/VM/intervalthread.cpp

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/CanNet/cannetmanager.o: src/CanNet/cannetmanager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CanNet/cannetmanager.o src/CanNet/cannetmanager.cpp

${OBJECTDIR}/src/VM/virtualmachine.o: src/VM/virtualmachine.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/VM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/VM/virtualmachine.o src/VM/virtualmachine.cpp

${OBJECTDIR}/src/VM/commandthread.o: src/VM/commandthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/VM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/VM/commandthread.o src/VM/commandthread.cpp

${OBJECTDIR}/src/singleton.o: src/singleton.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/singleton.o src/singleton.cpp

${OBJECTDIR}/src/VM/socketthread.o: src/VM/socketthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/VM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/VM/socketthread.o src/VM/socketthread.cpp

${OBJECTDIR}/src/CanNet/canidtranslator.o: src/CanNet/canidtranslator.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CanNet/canidtranslator.o src/CanNet/canidtranslator.cpp

${OBJECTDIR}/src/CanNet/candebug.o: src/CanNet/candebug.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CanNet/candebug.o src/CanNet/candebug.cpp

${OBJECTDIR}/src/Xml/xmlnode.o: src/Xml/xmlnode.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Xml
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Xml/xmlnode.o src/Xml/xmlnode.cpp

${OBJECTDIR}/src/Tools/tools.o: src/Tools/tools.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Tools
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Tools/tools.o src/Tools/tools.cpp

${OBJECTDIR}/src/Settings/settings.o: src/Settings/settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Settings
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Settings/settings.o src/Settings/settings.cpp

${OBJECTDIR}/src/Socket/asyncsocket.o: src/Socket/asyncsocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Socket
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Socket/asyncsocket.o src/Socket/asyncsocket.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} atom

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
