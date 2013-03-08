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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/robot.o \
	${OBJECTDIR}/utils.o \
	${OBJECTDIR}/simulator.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-I . `pkg-config --cflags playerc++`  
CXXFLAGS=-I . `pkg-config --cflags playerc++`  

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/include/player-3.1 `pkg-config --libs playerc++`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/prac1

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/prac1: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} `pkg-config --libs playerc++ ` -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/prac1 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/robot.o: robot.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g `pkg-config --cflags playerc++`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/robot.o robot.cpp

${OBJECTDIR}/utils.o: utils.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g `pkg-config --cflags playerc++`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/utils.o utils.cpp

${OBJECTDIR}/simulator.o: simulator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g `pkg-config --cflags playerc++`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/simulator.o simulator.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/prac1

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
