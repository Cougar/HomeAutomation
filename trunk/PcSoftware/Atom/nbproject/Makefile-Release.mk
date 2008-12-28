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
	${OBJECTDIR}/VM/virtualmachine.o \
	${OBJECTDIR}/_ext/home/migan/NetBeansProjects/Atom/../../../../NetBeansProjects/Atom/CanNet/candebug.o \
	${OBJECTDIR}/CanNet/cannetmanager.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Xml/xmlnode.o \
	${OBJECTDIR}/VM/intervalthread.o \
	${OBJECTDIR}/Settings/settings.o \
	${OBJECTDIR}/CanNet/canidtranslator.o \
	${OBJECTDIR}/CanNet/canmessage.o \
	${OBJECTDIR}/Tools/tools.o \
	${OBJECTDIR}/VM/socketthread.o \
	${OBJECTDIR}/SyslogStream/syslogstream.o \
	${OBJECTDIR}/Socket/asyncsocket.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread v8/libv8.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk atom

atom: v8/libv8.a

atom: ${OBJECTFILES}
	${LINK.cc} -m32 -o atom ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/VM/virtualmachine.o: VM/virtualmachine.cpp 
	${MKDIR} -p ${OBJECTDIR}/VM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/VM/virtualmachine.o VM/virtualmachine.cpp

${OBJECTDIR}/_ext/home/migan/NetBeansProjects/Atom/../../../../NetBeansProjects/Atom/CanNet/candebug.o: ../../../../NetBeansProjects/Atom/CanNet/candebug.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/migan/NetBeansProjects/Atom/../../../../NetBeansProjects/Atom/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/home/migan/NetBeansProjects/Atom/../../../../NetBeansProjects/Atom/CanNet/candebug.o ../../../../NetBeansProjects/Atom/CanNet/candebug.cpp

${OBJECTDIR}/CanNet/cannetmanager.o: CanNet/cannetmanager.cpp 
	${MKDIR} -p ${OBJECTDIR}/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/CanNet/cannetmanager.o CanNet/cannetmanager.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Xml/xmlnode.o: Xml/xmlnode.cpp 
	${MKDIR} -p ${OBJECTDIR}/Xml
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Xml/xmlnode.o Xml/xmlnode.cpp

${OBJECTDIR}/VM/intervalthread.o: VM/intervalthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/VM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/VM/intervalthread.o VM/intervalthread.cpp

${OBJECTDIR}/Settings/settings.o: Settings/settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/Settings
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Settings/settings.o Settings/settings.cpp

${OBJECTDIR}/CanNet/canidtranslator.o: CanNet/canidtranslator.cpp 
	${MKDIR} -p ${OBJECTDIR}/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/CanNet/canidtranslator.o CanNet/canidtranslator.cpp

${OBJECTDIR}/CanNet/canmessage.o: CanNet/canmessage.cpp 
	${MKDIR} -p ${OBJECTDIR}/CanNet
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/CanNet/canmessage.o CanNet/canmessage.cpp

${OBJECTDIR}/Tools/tools.o: Tools/tools.cpp 
	${MKDIR} -p ${OBJECTDIR}/Tools
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Tools/tools.o Tools/tools.cpp

${OBJECTDIR}/VM/socketthread.o: VM/socketthread.cpp 
	${MKDIR} -p ${OBJECTDIR}/VM
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/VM/socketthread.o VM/socketthread.cpp

${OBJECTDIR}/SyslogStream/syslogstream.o: SyslogStream/syslogstream.cpp 
	${MKDIR} -p ${OBJECTDIR}/SyslogStream
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/SyslogStream/syslogstream.o SyslogStream/syslogstream.cpp

${OBJECTDIR}/Socket/asyncsocket.o: Socket/asyncsocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/Socket
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/Socket/asyncsocket.o Socket/asyncsocket.cpp

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
