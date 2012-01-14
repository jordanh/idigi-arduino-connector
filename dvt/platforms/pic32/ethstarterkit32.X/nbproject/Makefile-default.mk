#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
# Adding MPLAB X bin directory to path
PATH:=/opt/microchip/mplabx/mplab_ide/mplab_ide/modules/../../bin/:$(PATH)
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1598139559/ARP.o ${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o ${OBJECTDIR}/_ext/1598139559/DHCP.o ${OBJECTDIR}/_ext/1598139559/DNS.o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o ${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o ${OBJECTDIR}/_ext/1598139559/Helpers.o ${OBJECTDIR}/_ext/1598139559/IP.o ${OBJECTDIR}/_ext/1598139559/SNTP.o ${OBJECTDIR}/_ext/1598139559/StackTsk.o ${OBJECTDIR}/_ext/1598139559/TCP.o ${OBJECTDIR}/_ext/1598139559/Tick.o ${OBJECTDIR}/_ext/1598139559/UDP.o ${OBJECTDIR}/_ext/1474906691/idigi_api.o ${OBJECTDIR}/config.o ${OBJECTDIR}/main.o ${OBJECTDIR}/network.o ${OBJECTDIR}/os.o ${OBJECTDIR}/data_service.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1598139559/ARP.o.d ${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o.d ${OBJECTDIR}/_ext/1598139559/DHCP.o.d ${OBJECTDIR}/_ext/1598139559/DNS.o.d ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o.d ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o.d ${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o.d ${OBJECTDIR}/_ext/1598139559/Helpers.o.d ${OBJECTDIR}/_ext/1598139559/IP.o.d ${OBJECTDIR}/_ext/1598139559/SNTP.o.d ${OBJECTDIR}/_ext/1598139559/StackTsk.o.d ${OBJECTDIR}/_ext/1598139559/TCP.o.d ${OBJECTDIR}/_ext/1598139559/Tick.o.d ${OBJECTDIR}/_ext/1598139559/UDP.o.d ${OBJECTDIR}/_ext/1474906691/idigi_api.o.d ${OBJECTDIR}/config.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/network.o.d ${OBJECTDIR}/os.o.d ${OBJECTDIR}/data_service.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1598139559/ARP.o ${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o ${OBJECTDIR}/_ext/1598139559/DHCP.o ${OBJECTDIR}/_ext/1598139559/DNS.o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o ${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o ${OBJECTDIR}/_ext/1598139559/Helpers.o ${OBJECTDIR}/_ext/1598139559/IP.o ${OBJECTDIR}/_ext/1598139559/SNTP.o ${OBJECTDIR}/_ext/1598139559/StackTsk.o ${OBJECTDIR}/_ext/1598139559/TCP.o ${OBJECTDIR}/_ext/1598139559/Tick.o ${OBJECTDIR}/_ext/1598139559/UDP.o ${OBJECTDIR}/_ext/1474906691/idigi_api.o ${OBJECTDIR}/config.o ${OBJECTDIR}/main.o ${OBJECTDIR}/network.o ${OBJECTDIR}/os.o ${OBJECTDIR}/data_service.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH="/usr/lib/jvm/java-6-sun-1.6.0.26/jre/bin/"
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC="/opt/microchip/mplabc32/v2.02/bin/pic32-gcc"
# MP_BC is not defined
MP_AS="/opt/microchip/mplabc32/v2.02/bin/pic32-as"
MP_LD="/opt/microchip/mplabc32/v2.02/bin/pic32-ld"
MP_AR="/opt/microchip/mplabc32/v2.02/bin/pic32-ar"
DEP_GEN=${MP_JAVA_PATH}java -jar "/opt/microchip/mplabx/mplab_ide/mplab_ide/modules/../../bin/extractobjectdependencies.jar" 
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps
MP_CC_DIR="/opt/microchip/mplabc32/v2.02/bin"
# MP_BC_DIR is not defined
MP_AS_DIR="/opt/microchip/mplabc32/v2.02/bin"
MP_LD_DIR="/opt/microchip/mplabc32/v2.02/bin"
MP_AR_DIR="/opt/microchip/mplabc32/v2.02/bin"
# MP_BC_DIR is not defined

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1598139559/ARP.o: /opt/microchip/Microchip/TCPIP\ Stack/ARP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ARP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ARP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ARP.o.d" -o ${OBJECTDIR}/_ext/1598139559/ARP.o "/opt/microchip/Microchip/TCPIP Stack/ARP.c"  
	
${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o: /opt/microchip/Microchip/TCPIP\ Stack/BerkeleyAPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o.d" -o ${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o "/opt/microchip/Microchip/TCPIP Stack/BerkeleyAPI.c"  
	
${OBJECTDIR}/_ext/1598139559/DHCP.o: /opt/microchip/Microchip/TCPIP\ Stack/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/DHCP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/DHCP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/DHCP.o.d" -o ${OBJECTDIR}/_ext/1598139559/DHCP.o "/opt/microchip/Microchip/TCPIP Stack/DHCP.c"  
	
${OBJECTDIR}/_ext/1598139559/DNS.o: /opt/microchip/Microchip/TCPIP\ Stack/DNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/DNS.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/DNS.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/DNS.o.d" -o ${OBJECTDIR}/_ext/1598139559/DNS.o "/opt/microchip/Microchip/TCPIP Stack/DNS.c"  
	
${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o: /opt/microchip/Microchip/TCPIP\ Stack/ETHPIC32ExtPhy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o.d" -o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o "/opt/microchip/Microchip/TCPIP Stack/ETHPIC32ExtPhy.c"  
	
${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o: /opt/microchip/Microchip/TCPIP\ Stack/ETHPIC32ExtPhyDP83640.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o.d" -o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o "/opt/microchip/Microchip/TCPIP Stack/ETHPIC32ExtPhyDP83640.c"  
	
${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o: /opt/microchip/Microchip/TCPIP\ Stack/ETHPIC32IntMac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o.d" -o ${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o "/opt/microchip/Microchip/TCPIP Stack/ETHPIC32IntMac.c"  
	
${OBJECTDIR}/_ext/1598139559/Helpers.o: /opt/microchip/Microchip/TCPIP\ Stack/Helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/Helpers.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/Helpers.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/Helpers.o.d" -o ${OBJECTDIR}/_ext/1598139559/Helpers.o "/opt/microchip/Microchip/TCPIP Stack/Helpers.c"  
	
${OBJECTDIR}/_ext/1598139559/IP.o: /opt/microchip/Microchip/TCPIP\ Stack/IP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/IP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/IP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/IP.o.d" -o ${OBJECTDIR}/_ext/1598139559/IP.o "/opt/microchip/Microchip/TCPIP Stack/IP.c"  
	
${OBJECTDIR}/_ext/1598139559/SNTP.o: /opt/microchip/Microchip/TCPIP\ Stack/SNTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/SNTP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/SNTP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/SNTP.o.d" -o ${OBJECTDIR}/_ext/1598139559/SNTP.o "/opt/microchip/Microchip/TCPIP Stack/SNTP.c"  
	
${OBJECTDIR}/_ext/1598139559/StackTsk.o: /opt/microchip/Microchip/TCPIP\ Stack/StackTsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/StackTsk.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/StackTsk.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/StackTsk.o.d" -o ${OBJECTDIR}/_ext/1598139559/StackTsk.o "/opt/microchip/Microchip/TCPIP Stack/StackTsk.c"  
	
${OBJECTDIR}/_ext/1598139559/TCP.o: /opt/microchip/Microchip/TCPIP\ Stack/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/TCP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/TCP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/TCP.o.d" -o ${OBJECTDIR}/_ext/1598139559/TCP.o "/opt/microchip/Microchip/TCPIP Stack/TCP.c"  
	
${OBJECTDIR}/_ext/1598139559/Tick.o: /opt/microchip/Microchip/TCPIP\ Stack/Tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/Tick.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/Tick.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/Tick.o.d" -o ${OBJECTDIR}/_ext/1598139559/Tick.o "/opt/microchip/Microchip/TCPIP Stack/Tick.c"  
	
${OBJECTDIR}/_ext/1598139559/UDP.o: /opt/microchip/Microchip/TCPIP\ Stack/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/UDP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/UDP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/UDP.o.d" -o ${OBJECTDIR}/_ext/1598139559/UDP.o "/opt/microchip/Microchip/TCPIP Stack/UDP.c"  
	
${OBJECTDIR}/_ext/1474906691/idigi_api.o: ../../../../private/idigi_api.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1474906691 
	@${RM} ${OBJECTDIR}/_ext/1474906691/idigi_api.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1474906691/idigi_api.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1474906691/idigi_api.o.d" -o ${OBJECTDIR}/_ext/1474906691/idigi_api.o ../../../../private/idigi_api.c  
	
${OBJECTDIR}/config.o: config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/config.o.d 
	@${FIXDEPS} "${OBJECTDIR}/config.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/config.o.d" -o ${OBJECTDIR}/config.o config.c  
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c  
	
${OBJECTDIR}/network.o: network.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/network.o.d 
	@${FIXDEPS} "${OBJECTDIR}/network.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/network.o.d" -o ${OBJECTDIR}/network.o network.c  
	
${OBJECTDIR}/os.o: os.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/os.o.d 
	@${FIXDEPS} "${OBJECTDIR}/os.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/os.o.d" -o ${OBJECTDIR}/os.o os.c  
	
${OBJECTDIR}/data_service.o: data_service.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/data_service.o.d 
	@${FIXDEPS} "${OBJECTDIR}/data_service.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/data_service.o.d" -o ${OBJECTDIR}/data_service.o data_service.c  
	
else
${OBJECTDIR}/_ext/1598139559/ARP.o: /opt/microchip/Microchip/TCPIP\ Stack/ARP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ARP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ARP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ARP.o.d" -o ${OBJECTDIR}/_ext/1598139559/ARP.o "/opt/microchip/Microchip/TCPIP Stack/ARP.c"  
	
${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o: /opt/microchip/Microchip/TCPIP\ Stack/BerkeleyAPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o.d" -o ${OBJECTDIR}/_ext/1598139559/BerkeleyAPI.o "/opt/microchip/Microchip/TCPIP Stack/BerkeleyAPI.c"  
	
${OBJECTDIR}/_ext/1598139559/DHCP.o: /opt/microchip/Microchip/TCPIP\ Stack/DHCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/DHCP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/DHCP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/DHCP.o.d" -o ${OBJECTDIR}/_ext/1598139559/DHCP.o "/opt/microchip/Microchip/TCPIP Stack/DHCP.c"  
	
${OBJECTDIR}/_ext/1598139559/DNS.o: /opt/microchip/Microchip/TCPIP\ Stack/DNS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/DNS.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/DNS.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/DNS.o.d" -o ${OBJECTDIR}/_ext/1598139559/DNS.o "/opt/microchip/Microchip/TCPIP Stack/DNS.c"  
	
${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o: /opt/microchip/Microchip/TCPIP\ Stack/ETHPIC32ExtPhy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o.d" -o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhy.o "/opt/microchip/Microchip/TCPIP Stack/ETHPIC32ExtPhy.c"  
	
${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o: /opt/microchip/Microchip/TCPIP\ Stack/ETHPIC32ExtPhyDP83640.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o.d" -o ${OBJECTDIR}/_ext/1598139559/ETHPIC32ExtPhyDP83640.o "/opt/microchip/Microchip/TCPIP Stack/ETHPIC32ExtPhyDP83640.c"  
	
${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o: /opt/microchip/Microchip/TCPIP\ Stack/ETHPIC32IntMac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o.d" -o ${OBJECTDIR}/_ext/1598139559/ETHPIC32IntMac.o "/opt/microchip/Microchip/TCPIP Stack/ETHPIC32IntMac.c"  
	
${OBJECTDIR}/_ext/1598139559/Helpers.o: /opt/microchip/Microchip/TCPIP\ Stack/Helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/Helpers.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/Helpers.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/Helpers.o.d" -o ${OBJECTDIR}/_ext/1598139559/Helpers.o "/opt/microchip/Microchip/TCPIP Stack/Helpers.c"  
	
${OBJECTDIR}/_ext/1598139559/IP.o: /opt/microchip/Microchip/TCPIP\ Stack/IP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/IP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/IP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/IP.o.d" -o ${OBJECTDIR}/_ext/1598139559/IP.o "/opt/microchip/Microchip/TCPIP Stack/IP.c"  
	
${OBJECTDIR}/_ext/1598139559/SNTP.o: /opt/microchip/Microchip/TCPIP\ Stack/SNTP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/SNTP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/SNTP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/SNTP.o.d" -o ${OBJECTDIR}/_ext/1598139559/SNTP.o "/opt/microchip/Microchip/TCPIP Stack/SNTP.c"  
	
${OBJECTDIR}/_ext/1598139559/StackTsk.o: /opt/microchip/Microchip/TCPIP\ Stack/StackTsk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/StackTsk.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/StackTsk.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/StackTsk.o.d" -o ${OBJECTDIR}/_ext/1598139559/StackTsk.o "/opt/microchip/Microchip/TCPIP Stack/StackTsk.c"  
	
${OBJECTDIR}/_ext/1598139559/TCP.o: /opt/microchip/Microchip/TCPIP\ Stack/TCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/TCP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/TCP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/TCP.o.d" -o ${OBJECTDIR}/_ext/1598139559/TCP.o "/opt/microchip/Microchip/TCPIP Stack/TCP.c"  
	
${OBJECTDIR}/_ext/1598139559/Tick.o: /opt/microchip/Microchip/TCPIP\ Stack/Tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/Tick.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/Tick.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/Tick.o.d" -o ${OBJECTDIR}/_ext/1598139559/Tick.o "/opt/microchip/Microchip/TCPIP Stack/Tick.c"  
	
${OBJECTDIR}/_ext/1598139559/UDP.o: /opt/microchip/Microchip/TCPIP\ Stack/UDP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1598139559 
	@${RM} ${OBJECTDIR}/_ext/1598139559/UDP.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1598139559/UDP.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1598139559/UDP.o.d" -o ${OBJECTDIR}/_ext/1598139559/UDP.o "/opt/microchip/Microchip/TCPIP Stack/UDP.c"  
	
${OBJECTDIR}/_ext/1474906691/idigi_api.o: ../../../../private/idigi_api.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1474906691 
	@${RM} ${OBJECTDIR}/_ext/1474906691/idigi_api.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1474906691/idigi_api.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/_ext/1474906691/idigi_api.o.d" -o ${OBJECTDIR}/_ext/1474906691/idigi_api.o ../../../../private/idigi_api.c  
	
${OBJECTDIR}/config.o: config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/config.o.d 
	@${FIXDEPS} "${OBJECTDIR}/config.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/config.o.d" -o ${OBJECTDIR}/config.o config.c  
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c  
	
${OBJECTDIR}/network.o: network.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/network.o.d 
	@${FIXDEPS} "${OBJECTDIR}/network.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/network.o.d" -o ${OBJECTDIR}/network.o network.c  
	
${OBJECTDIR}/os.o: os.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/os.o.d 
	@${FIXDEPS} "${OBJECTDIR}/os.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/os.o.d" -o ${OBJECTDIR}/os.o os.c  
	
${OBJECTDIR}/data_service.o: data_service.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/data_service.o.d 
	@${FIXDEPS} "${OBJECTDIR}/data_service.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -D_LITTLE_ENDIAN -DIDIGI_VERSION=0x1010000UL -I"../../../../public/include" -I"/opt/microchip/Microchip/Include" -I"." -Werror -MMD -MF "${OBJECTDIR}/data_service.o.d" -o ${OBJECTDIR}/data_service.o data_service.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PIC32MXSK=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PIC32MXSK=1,--defsym=_min_heap_size=50000,--defsym=_min_stack_size=4096,--report-mem 
else
dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=50000,--defsym=_min_stack_size=4096,--report-mem
	${MP_CC_DIR}/pic32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "/opt/microchip/mplabx/mplab_ide/mplab_ide/modules/../../bin/"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
