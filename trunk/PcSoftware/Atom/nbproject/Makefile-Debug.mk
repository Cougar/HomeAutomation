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
OBJECTDIR=src/build/Debug/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/VM/virtualmachine.o \
	${OBJECTDIR}/Logger/logger.o \
	${OBJECTDIR}/Socket/server.o \
	${OBJECTDIR}/singleton.o \
	${OBJECTDIR}/CanNet/cannetmanager.o \
	${OBJECTDIR}/VM/commandthread.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Xml/xmlnode.o \
	${OBJECTDIR}/VM/intervalthread.o \
	${OBJECTDIR}/Settings/settings.o \
	${OBJECTDIR}/CanNet/canidtranslator.o \
	${OBJECTDIR}/CanNet/canmessage.o \
	${OBJECTDIR}/CanNet/candebug.o \
	${OBJECTDIR}/Tools/tools.o \
	${OBJECTDIR}/VM/socketthread.o \
	${OBJECTDIR}/Socket/asyncsocket.o

# C Compiler Flags
CFLAGS=-m32

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread src/v8/libv8.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk atom

atom: src/v8/libv8.a

atom: ${OBJECTFILES}
	${LINK.cc} -m32 -o atom ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/VM/virtualmachine.o: src/VM/virtualmachine.cpp 
	${MKDIR} -p ${OBJECTDIR}/VM
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/VM/virtualmachine.o src/VM/virtualmachine.cpp

${OBJECTDIR}/Logger/logger.o: src/Logger/logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/Logger
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Logger/logger.o src/Logger/logger.cpp

${OBJECTDIR}/Socket/server.o: src/Socket/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/Socket
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Socket/server.o src/Socket/server.cpp

${OBJECTDIR}/singleton.o: src/singleton.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/singleton.o src/singleton.cpp

${OBJECTDIR}/CanNet/cannetmanager.o: src/CanNet/cannetmanager.cpp 
	${MKDIR} -p ${OBJECTDIR}/CanNet
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CanNet/cannetmanager.o src/CanNet/cannetmanager.cpp

${OBJECTDIR}/VM/commandthread.o: src/VM/commandthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/VM
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/VM/commandthread.o src/VM/commandthread.cpp

${OBJECTDIR}/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o src/main.cpp

${OBJECTDIR}/Xml/xmlnode.o: src/Xml/xmlnode.cpp 
	${MKDIR} -p ${OBJECTDIR}/Xml
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Xml/xmlnode.o src/Xml/xmlnode.cpp

${OBJECTDIR}/VM/intervalthread.o: src/VM/intervalthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/VM
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/VM/intervalthread.o src/VM/intervalthread.cpp

${OBJECTDIR}/Settings/settings.o: src/Settings/settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/Settings
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Settings/settings.o src/Settings/settings.cpp

${OBJECTDIR}/CanNet/canidtranslator.o: src/CanNet/canidtranslator.cpp 
	${MKDIR} -p ${OBJECTDIR}/CanNet
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CanNet/canidtranslator.o src/CanNet/canidtranslator.cpp

${OBJECTDIR}/CanNet/canmessage.o: src/CanNet/canmessage.cpp 
	${MKDIR} -p ${OBJECTDIR}/CanNet
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CanNet/canmessage.o src/CanNet/canmessage.cpp

${OBJECTDIR}/CanNet/candebug.o: src/CanNet/candebug.cpp 
	${MKDIR} -p ${OBJECTDIR}/CanNet
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CanNet/candebug.o src/CanNet/candebug.cpp

${OBJECTDIR}/Tools/tools.o: src/Tools/tools.cpp 
	${MKDIR} -p ${OBJECTDIR}/Tools
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Tools/tools.o src/Tools/tools.cpp

${OBJECTDIR}/VM/socketthread.o: src/VM/socketthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/VM
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/VM/socketthread.o src/VM/socketthread.cpp

${OBJECTDIR}/Socket/asyncsocket.o: src/Socket/asyncsocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/Socket
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Socket/asyncsocket.o src/Socket/asyncsocket.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r src/build/Debug
	${RM} atom

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
