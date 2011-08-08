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
MKDIR=mkdir -p
RM=rm -f 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/config.o ${OBJECTDIR}/_ext/1360937237/firmware.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/network.o ${OBJECTDIR}/_ext/1360937237/os.o ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o ${OBJECTDIR}/_ext/1360937237/rci.o ${OBJECTDIR}/_ext/1360937237/system_services.o ${OBJECTDIR}/_ext/1586745794/block_mgr.o ${OBJECTDIR}/_ext/1586745794/buff_mgr.o ${OBJECTDIR}/_ext/1586745794/earp.o ${OBJECTDIR}/_ext/1586745794/eicmp.o ${OBJECTDIR}/_ext/1586745794/eip.o ${OBJECTDIR}/_ext/1586745794/emac.o ${OBJECTDIR}/_ext/1586745794/enc28j60.o ${OBJECTDIR}/_ext/1586745794/etcp.o ${OBJECTDIR}/_ext/1586745794/ether.o ${OBJECTDIR}/_ext/1586745794/eudp.o ${OBJECTDIR}/_ext/1586745794/gpfunc.o ${OBJECTDIR}/_ext/1586745794/mc_socket.o ${OBJECTDIR}/_ext/1586745794/pkt_queue.o ${OBJECTDIR}/_ext/1586745794/route.o ${OBJECTDIR}/_ext/1586745794/stackmgr.o ${OBJECTDIR}/_ext/1586745794/tcp_tick.o ${OBJECTDIR}/_ext/1586745794/tcpip_events.o ${OBJECTDIR}/_ext/1958230400/dhcp.o ${OBJECTDIR}/_ext/320086670/eth_phy.o ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH=/System/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Home/bin/
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC=/Applications/microchip/mplabc32/v2.00/bin/pic32-gcc
# MP_BC is not defined
MP_AS=/Applications/microchip/mplabc32/v2.00/bin/pic32-as
MP_LD=/Applications/microchip/mplabc32/v2.00/bin/pic32-ld
MP_AR=/Applications/microchip/mplabc32/v2.00/bin/pic32-ar
# MP_BC is not defined
MP_CC_DIR=/Applications/microchip/mplabc32/v2.00/bin
# MP_BC_DIR is not defined
MP_AS_DIR=/Applications/microchip/mplabc32/v2.00/bin
MP_LD_DIR=/Applications/microchip/mplabc32/v2.00/bin
MP_AR_DIR=/Applications/microchip/mplabc32/v2.00/bin
# MP_BC_DIR is not defined

.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf

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
${OBJECTDIR}/_ext/1586745794/stackmgr.o: ../src/tcpip_bsd/stackmgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok ${OBJECTDIR}/_ext/1586745794/stackmgr.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d -o ${OBJECTDIR}/_ext/1586745794/stackmgr.o ../src/tcpip_bsd/stackmgr.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d -o ${OBJECTDIR}/_ext/1586745794/stackmgr.o ../src/tcpip_bsd/stackmgr.c   > ${OBJECTDIR}/_ext/1586745794/stackmgr.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d > ${OBJECTDIR}/_ext/1586745794/stackmgr.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.tmp ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/stackmgr.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/earp.o: ../src/tcpip_bsd/earp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.ok ${OBJECTDIR}/_ext/1586745794/earp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/earp.o.d -o ${OBJECTDIR}/_ext/1586745794/earp.o ../src/tcpip_bsd/earp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/earp.o.d -o ${OBJECTDIR}/_ext/1586745794/earp.o ../src/tcpip_bsd/earp.c   > ${OBJECTDIR}/_ext/1586745794/earp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/earp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/earp.o.d > ${OBJECTDIR}/_ext/1586745794/earp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/earp.o.tmp ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/earp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/earp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/earp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/eicmp.o: ../src/tcpip_bsd/eicmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok ${OBJECTDIR}/_ext/1586745794/eicmp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eicmp.o.d -o ${OBJECTDIR}/_ext/1586745794/eicmp.o ../src/tcpip_bsd/eicmp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eicmp.o.d -o ${OBJECTDIR}/_ext/1586745794/eicmp.o ../src/tcpip_bsd/eicmp.c   > ${OBJECTDIR}/_ext/1586745794/eicmp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/eicmp.o.d > ${OBJECTDIR}/_ext/1586745794/eicmp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/eicmp.o.tmp ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/eicmp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/rci.o: ../src/rci.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.ok ${OBJECTDIR}/_ext/1360937237/rci.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/rci.o.d -o ${OBJECTDIR}/_ext/1360937237/rci.o ../src/rci.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/rci.o.d -o ${OBJECTDIR}/_ext/1360937237/rci.o ../src/rci.c   > ${OBJECTDIR}/_ext/1360937237/rci.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/rci.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/rci.o.d > ${OBJECTDIR}/_ext/1360937237/rci.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/rci.o.tmp ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/rci.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/rci.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/rci.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/tcp_tick.o: ../src/tcpip_bsd/tcp_tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d -o ${OBJECTDIR}/_ext/1586745794/tcp_tick.o ../src/tcpip_bsd/tcp_tick.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d -o ${OBJECTDIR}/_ext/1586745794/tcp_tick.o ../src/tcpip_bsd/tcp_tick.c   > ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d > ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.tmp ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/block_mgr.o: ../src/tcpip_bsd/block_mgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok ${OBJECTDIR}/_ext/1586745794/block_mgr.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/block_mgr.o ../src/tcpip_bsd/block_mgr.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/block_mgr.o ../src/tcpip_bsd/block_mgr.c   > ${OBJECTDIR}/_ext/1586745794/block_mgr.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d > ${OBJECTDIR}/_ext/1586745794/block_mgr.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.tmp ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/block_mgr.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/config.o: ../src/config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.ok ${OBJECTDIR}/_ext/1360937237/config.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/config.o.d -o ${OBJECTDIR}/_ext/1360937237/config.o ../src/config.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/config.o.d -o ${OBJECTDIR}/_ext/1360937237/config.o ../src/config.c   > ${OBJECTDIR}/_ext/1360937237/config.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/config.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/config.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/config.o.d > ${OBJECTDIR}/_ext/1360937237/config.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/config.o.tmp ${OBJECTDIR}/_ext/1360937237/config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/config.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/config.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/config.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/etcp.o: ../src/tcpip_bsd/etcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.ok ${OBJECTDIR}/_ext/1586745794/etcp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/etcp.o.d -o ${OBJECTDIR}/_ext/1586745794/etcp.o ../src/tcpip_bsd/etcp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/etcp.o.d -o ${OBJECTDIR}/_ext/1586745794/etcp.o ../src/tcpip_bsd/etcp.c   > ${OBJECTDIR}/_ext/1586745794/etcp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/etcp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/etcp.o.d > ${OBJECTDIR}/_ext/1586745794/etcp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/etcp.o.tmp ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/etcp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/etcp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/etcp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/eip.o: ../src/tcpip_bsd/eip.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.ok ${OBJECTDIR}/_ext/1586745794/eip.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eip.o.d -o ${OBJECTDIR}/_ext/1586745794/eip.o ../src/tcpip_bsd/eip.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eip.o.d -o ${OBJECTDIR}/_ext/1586745794/eip.o ../src/tcpip_bsd/eip.c   > ${OBJECTDIR}/_ext/1586745794/eip.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/eip.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/eip.o.d > ${OBJECTDIR}/_ext/1586745794/eip.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/eip.o.tmp ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/eip.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/eip.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/eip.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/320086670/eth_phy.o: ../src/tcpip_bsd/eth_phy/eth_phy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/320086670 
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok ${OBJECTDIR}/_ext/320086670/eth_phy.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/eth_phy.o.d -o ${OBJECTDIR}/_ext/320086670/eth_phy.o ../src/tcpip_bsd/eth_phy/eth_phy.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/eth_phy.o.d -o ${OBJECTDIR}/_ext/320086670/eth_phy.o ../src/tcpip_bsd/eth_phy/eth_phy.c   > ${OBJECTDIR}/_ext/320086670/eth_phy.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/320086670/eth_phy.o.d > ${OBJECTDIR}/_ext/320086670/eth_phy.o.tmp
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	@${CP} ${OBJECTDIR}/_ext/320086670/eth_phy.o.tmp ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok; else cat ${OBJECTDIR}/_ext/320086670/eth_phy.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/mc_socket.o: ../src/tcpip_bsd/mc_socket.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok ${OBJECTDIR}/_ext/1586745794/mc_socket.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d -o ${OBJECTDIR}/_ext/1586745794/mc_socket.o ../src/tcpip_bsd/mc_socket.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d -o ${OBJECTDIR}/_ext/1586745794/mc_socket.o ../src/tcpip_bsd/mc_socket.c   > ${OBJECTDIR}/_ext/1586745794/mc_socket.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d > ${OBJECTDIR}/_ext/1586745794/mc_socket.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.tmp ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/mc_socket.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/system_services.o: ../src/system_services.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.ok ${OBJECTDIR}/_ext/1360937237/system_services.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/system_services.o.d -o ${OBJECTDIR}/_ext/1360937237/system_services.o ../src/system_services.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/system_services.o.d -o ${OBJECTDIR}/_ext/1360937237/system_services.o ../src/system_services.c   > ${OBJECTDIR}/_ext/1360937237/system_services.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/system_services.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/system_services.o.d > ${OBJECTDIR}/_ext/1360937237/system_services.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/system_services.o.tmp ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/system_services.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/system_services.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/system_services.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/eudp.o: ../src/tcpip_bsd/eudp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.ok ${OBJECTDIR}/_ext/1586745794/eudp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eudp.o.d -o ${OBJECTDIR}/_ext/1586745794/eudp.o ../src/tcpip_bsd/eudp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eudp.o.d -o ${OBJECTDIR}/_ext/1586745794/eudp.o ../src/tcpip_bsd/eudp.c   > ${OBJECTDIR}/_ext/1586745794/eudp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/eudp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/eudp.o.d > ${OBJECTDIR}/_ext/1586745794/eudp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/eudp.o.tmp ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/eudp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/eudp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/eudp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/gpfunc.o: ../src/tcpip_bsd/gpfunc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok ${OBJECTDIR}/_ext/1586745794/gpfunc.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d -o ${OBJECTDIR}/_ext/1586745794/gpfunc.o ../src/tcpip_bsd/gpfunc.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d -o ${OBJECTDIR}/_ext/1586745794/gpfunc.o ../src/tcpip_bsd/gpfunc.c   > ${OBJECTDIR}/_ext/1586745794/gpfunc.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d > ${OBJECTDIR}/_ext/1586745794/gpfunc.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.tmp ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/gpfunc.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/os.o: ../src/os.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.ok ${OBJECTDIR}/_ext/1360937237/os.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/os.o.d -o ${OBJECTDIR}/_ext/1360937237/os.o ../src/os.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/os.o.d -o ${OBJECTDIR}/_ext/1360937237/os.o ../src/os.c   > ${OBJECTDIR}/_ext/1360937237/os.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/os.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/os.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/os.o.d > ${OBJECTDIR}/_ext/1360937237/os.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/os.o.tmp ${OBJECTDIR}/_ext/1360937237/os.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/os.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/os.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/os.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/320086670/nat_dp83848c.o: ../src/tcpip_bsd/eth_phy/nat_dp83848c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/320086670 
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d -o ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o ../src/tcpip_bsd/eth_phy/nat_dp83848c.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d -o ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o ../src/tcpip_bsd/eth_phy/nat_dp83848c.c   > ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d > ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.tmp
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	@${CP} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.tmp ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok; else cat ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/network.o: ../src/network.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.ok ${OBJECTDIR}/_ext/1360937237/network.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/network.o.d -o ${OBJECTDIR}/_ext/1360937237/network.o ../src/network.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/network.o.d -o ${OBJECTDIR}/_ext/1360937237/network.o ../src/network.c   > ${OBJECTDIR}/_ext/1360937237/network.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/network.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/network.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/network.o.d > ${OBJECTDIR}/_ext/1360937237/network.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/network.o.tmp ${OBJECTDIR}/_ext/1360937237/network.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/network.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/network.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/network.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/route.o: ../src/tcpip_bsd/route.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.ok ${OBJECTDIR}/_ext/1586745794/route.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/route.o.d -o ${OBJECTDIR}/_ext/1586745794/route.o ../src/tcpip_bsd/route.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/route.o.d -o ${OBJECTDIR}/_ext/1586745794/route.o ../src/tcpip_bsd/route.c   > ${OBJECTDIR}/_ext/1586745794/route.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/route.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/route.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/route.o.d > ${OBJECTDIR}/_ext/1586745794/route.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/route.o.tmp ${OBJECTDIR}/_ext/1586745794/route.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/route.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/route.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/route.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/buff_mgr.o: ../src/tcpip_bsd/buff_mgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/buff_mgr.o ../src/tcpip_bsd/buff_mgr.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/buff_mgr.o ../src/tcpip_bsd/buff_mgr.c   > ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d > ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.tmp ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/enc28j60.o: ../src/tcpip_bsd/enc28j60.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok ${OBJECTDIR}/_ext/1586745794/enc28j60.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d -o ${OBJECTDIR}/_ext/1586745794/enc28j60.o ../src/tcpip_bsd/enc28j60.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d -o ${OBJECTDIR}/_ext/1586745794/enc28j60.o ../src/tcpip_bsd/enc28j60.c   > ${OBJECTDIR}/_ext/1586745794/enc28j60.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d > ${OBJECTDIR}/_ext/1586745794/enc28j60.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.tmp ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/enc28j60.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o: ../src/pic32mx_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d -o ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o ../src/pic32mx_cfg.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d -o ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o ../src/pic32mx_cfg.c   > ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d > ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.tmp ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1958230400/dhcp.o: ../src/tcpip_bsd/bsd_dhcp_client/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1958230400 
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok ${OBJECTDIR}/_ext/1958230400/dhcp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1958230400/dhcp.o.d -o ${OBJECTDIR}/_ext/1958230400/dhcp.o ../src/tcpip_bsd/bsd_dhcp_client/dhcp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1958230400/dhcp.o.d -o ${OBJECTDIR}/_ext/1958230400/dhcp.o ../src/tcpip_bsd/bsd_dhcp_client/dhcp.c   > ${OBJECTDIR}/_ext/1958230400/dhcp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1958230400/dhcp.o.d > ${OBJECTDIR}/_ext/1958230400/dhcp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1958230400/dhcp.o.tmp ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok; else cat ${OBJECTDIR}/_ext/1958230400/dhcp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/firmware.o: ../src/firmware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.ok ${OBJECTDIR}/_ext/1360937237/firmware.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/firmware.o.d -o ${OBJECTDIR}/_ext/1360937237/firmware.o ../src/firmware.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/firmware.o.d -o ${OBJECTDIR}/_ext/1360937237/firmware.o ../src/firmware.c   > ${OBJECTDIR}/_ext/1360937237/firmware.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/firmware.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/firmware.o.d > ${OBJECTDIR}/_ext/1360937237/firmware.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/firmware.o.tmp ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/firmware.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/firmware.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/firmware.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/pkt_queue.o: ../src/tcpip_bsd/pkt_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d -o ${OBJECTDIR}/_ext/1586745794/pkt_queue.o ../src/tcpip_bsd/pkt_queue.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d -o ${OBJECTDIR}/_ext/1586745794/pkt_queue.o ../src/tcpip_bsd/pkt_queue.c   > ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d > ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.tmp ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/emac.o: ../src/tcpip_bsd/emac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.ok ${OBJECTDIR}/_ext/1586745794/emac.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/emac.o.d -o ${OBJECTDIR}/_ext/1586745794/emac.o ../src/tcpip_bsd/emac.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/emac.o.d -o ${OBJECTDIR}/_ext/1586745794/emac.o ../src/tcpip_bsd/emac.c   > ${OBJECTDIR}/_ext/1586745794/emac.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/emac.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/emac.o.d > ${OBJECTDIR}/_ext/1586745794/emac.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/emac.o.tmp ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/emac.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/emac.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/emac.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/tcpip_events.o: ../src/tcpip_bsd/tcpip_events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d -o ${OBJECTDIR}/_ext/1586745794/tcpip_events.o ../src/tcpip_bsd/tcpip_events.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d -o ${OBJECTDIR}/_ext/1586745794/tcpip_events.o ../src/tcpip_bsd/tcpip_events.c   > ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d > ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.tmp ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.ok ${OBJECTDIR}/_ext/1360937237/main.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/main.o.d -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/main.o.d -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c   > ${OBJECTDIR}/_ext/1360937237/main.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/main.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/main.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/main.o.d > ${OBJECTDIR}/_ext/1360937237/main.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/main.o.tmp ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/main.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/main.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/main.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/ether.o: ../src/tcpip_bsd/ether.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.ok ${OBJECTDIR}/_ext/1586745794/ether.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/ether.o.d -o ${OBJECTDIR}/_ext/1586745794/ether.o ../src/tcpip_bsd/ether.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/ether.o.d -o ${OBJECTDIR}/_ext/1586745794/ether.o ../src/tcpip_bsd/ether.c   > ${OBJECTDIR}/_ext/1586745794/ether.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/ether.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/ether.o.d > ${OBJECTDIR}/_ext/1586745794/ether.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/ether.o.tmp ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/ether.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/ether.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/ether.o.err; exit 1; fi
	
else
${OBJECTDIR}/_ext/1586745794/stackmgr.o: ../src/tcpip_bsd/stackmgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok ${OBJECTDIR}/_ext/1586745794/stackmgr.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d -o ${OBJECTDIR}/_ext/1586745794/stackmgr.o ../src/tcpip_bsd/stackmgr.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d -o ${OBJECTDIR}/_ext/1586745794/stackmgr.o ../src/tcpip_bsd/stackmgr.c   > ${OBJECTDIR}/_ext/1586745794/stackmgr.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d > ${OBJECTDIR}/_ext/1586745794/stackmgr.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.tmp ${OBJECTDIR}/_ext/1586745794/stackmgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/stackmgr.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/stackmgr.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/stackmgr.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/earp.o: ../src/tcpip_bsd/earp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.ok ${OBJECTDIR}/_ext/1586745794/earp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/earp.o.d -o ${OBJECTDIR}/_ext/1586745794/earp.o ../src/tcpip_bsd/earp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/earp.o.d -o ${OBJECTDIR}/_ext/1586745794/earp.o ../src/tcpip_bsd/earp.c   > ${OBJECTDIR}/_ext/1586745794/earp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/earp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/earp.o.d > ${OBJECTDIR}/_ext/1586745794/earp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/earp.o.tmp ${OBJECTDIR}/_ext/1586745794/earp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/earp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/earp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/earp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/earp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/eicmp.o: ../src/tcpip_bsd/eicmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok ${OBJECTDIR}/_ext/1586745794/eicmp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eicmp.o.d -o ${OBJECTDIR}/_ext/1586745794/eicmp.o ../src/tcpip_bsd/eicmp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eicmp.o.d -o ${OBJECTDIR}/_ext/1586745794/eicmp.o ../src/tcpip_bsd/eicmp.c   > ${OBJECTDIR}/_ext/1586745794/eicmp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/eicmp.o.d > ${OBJECTDIR}/_ext/1586745794/eicmp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/eicmp.o.tmp ${OBJECTDIR}/_ext/1586745794/eicmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eicmp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/eicmp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/eicmp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/rci.o: ../src/rci.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.ok ${OBJECTDIR}/_ext/1360937237/rci.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/rci.o.d -o ${OBJECTDIR}/_ext/1360937237/rci.o ../src/rci.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/rci.o.d -o ${OBJECTDIR}/_ext/1360937237/rci.o ../src/rci.c   > ${OBJECTDIR}/_ext/1360937237/rci.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/rci.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/rci.o.d > ${OBJECTDIR}/_ext/1360937237/rci.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/rci.o.tmp ${OBJECTDIR}/_ext/1360937237/rci.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/rci.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/rci.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/rci.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/rci.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/tcp_tick.o: ../src/tcpip_bsd/tcp_tick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d -o ${OBJECTDIR}/_ext/1586745794/tcp_tick.o ../src/tcpip_bsd/tcp_tick.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d -o ${OBJECTDIR}/_ext/1586745794/tcp_tick.o ../src/tcpip_bsd/tcp_tick.c   > ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d > ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.tmp ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/tcp_tick.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/block_mgr.o: ../src/tcpip_bsd/block_mgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok ${OBJECTDIR}/_ext/1586745794/block_mgr.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/block_mgr.o ../src/tcpip_bsd/block_mgr.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/block_mgr.o ../src/tcpip_bsd/block_mgr.c   > ${OBJECTDIR}/_ext/1586745794/block_mgr.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d > ${OBJECTDIR}/_ext/1586745794/block_mgr.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.tmp ${OBJECTDIR}/_ext/1586745794/block_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/block_mgr.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/block_mgr.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/block_mgr.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/config.o: ../src/config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.ok ${OBJECTDIR}/_ext/1360937237/config.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/config.o.d -o ${OBJECTDIR}/_ext/1360937237/config.o ../src/config.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/config.o.d -o ${OBJECTDIR}/_ext/1360937237/config.o ../src/config.c   > ${OBJECTDIR}/_ext/1360937237/config.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/config.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/config.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/config.o.d > ${OBJECTDIR}/_ext/1360937237/config.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/config.o.tmp ${OBJECTDIR}/_ext/1360937237/config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/config.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/config.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/config.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/config.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/etcp.o: ../src/tcpip_bsd/etcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.ok ${OBJECTDIR}/_ext/1586745794/etcp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/etcp.o.d -o ${OBJECTDIR}/_ext/1586745794/etcp.o ../src/tcpip_bsd/etcp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/etcp.o.d -o ${OBJECTDIR}/_ext/1586745794/etcp.o ../src/tcpip_bsd/etcp.c   > ${OBJECTDIR}/_ext/1586745794/etcp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/etcp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/etcp.o.d > ${OBJECTDIR}/_ext/1586745794/etcp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/etcp.o.tmp ${OBJECTDIR}/_ext/1586745794/etcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/etcp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/etcp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/etcp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/etcp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/eip.o: ../src/tcpip_bsd/eip.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.ok ${OBJECTDIR}/_ext/1586745794/eip.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eip.o.d -o ${OBJECTDIR}/_ext/1586745794/eip.o ../src/tcpip_bsd/eip.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eip.o.d -o ${OBJECTDIR}/_ext/1586745794/eip.o ../src/tcpip_bsd/eip.c   > ${OBJECTDIR}/_ext/1586745794/eip.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/eip.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/eip.o.d > ${OBJECTDIR}/_ext/1586745794/eip.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/eip.o.tmp ${OBJECTDIR}/_ext/1586745794/eip.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eip.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/eip.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/eip.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/eip.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/320086670/eth_phy.o: ../src/tcpip_bsd/eth_phy/eth_phy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/320086670 
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok ${OBJECTDIR}/_ext/320086670/eth_phy.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/eth_phy.o.d -o ${OBJECTDIR}/_ext/320086670/eth_phy.o ../src/tcpip_bsd/eth_phy/eth_phy.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/eth_phy.o.d -o ${OBJECTDIR}/_ext/320086670/eth_phy.o ../src/tcpip_bsd/eth_phy/eth_phy.c   > ${OBJECTDIR}/_ext/320086670/eth_phy.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/320086670/eth_phy.o.d > ${OBJECTDIR}/_ext/320086670/eth_phy.o.tmp
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	@${CP} ${OBJECTDIR}/_ext/320086670/eth_phy.o.tmp ${OBJECTDIR}/_ext/320086670/eth_phy.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/eth_phy.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/320086670/eth_phy.o.ok; else cat ${OBJECTDIR}/_ext/320086670/eth_phy.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/mc_socket.o: ../src/tcpip_bsd/mc_socket.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok ${OBJECTDIR}/_ext/1586745794/mc_socket.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d -o ${OBJECTDIR}/_ext/1586745794/mc_socket.o ../src/tcpip_bsd/mc_socket.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d -o ${OBJECTDIR}/_ext/1586745794/mc_socket.o ../src/tcpip_bsd/mc_socket.c   > ${OBJECTDIR}/_ext/1586745794/mc_socket.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d > ${OBJECTDIR}/_ext/1586745794/mc_socket.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.tmp ${OBJECTDIR}/_ext/1586745794/mc_socket.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/mc_socket.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/mc_socket.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/mc_socket.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/system_services.o: ../src/system_services.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.ok ${OBJECTDIR}/_ext/1360937237/system_services.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/system_services.o.d -o ${OBJECTDIR}/_ext/1360937237/system_services.o ../src/system_services.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/system_services.o.d -o ${OBJECTDIR}/_ext/1360937237/system_services.o ../src/system_services.c   > ${OBJECTDIR}/_ext/1360937237/system_services.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/system_services.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/system_services.o.d > ${OBJECTDIR}/_ext/1360937237/system_services.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/system_services.o.tmp ${OBJECTDIR}/_ext/1360937237/system_services.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/system_services.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/system_services.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/system_services.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/system_services.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/eudp.o: ../src/tcpip_bsd/eudp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.ok ${OBJECTDIR}/_ext/1586745794/eudp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eudp.o.d -o ${OBJECTDIR}/_ext/1586745794/eudp.o ../src/tcpip_bsd/eudp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/eudp.o.d -o ${OBJECTDIR}/_ext/1586745794/eudp.o ../src/tcpip_bsd/eudp.c   > ${OBJECTDIR}/_ext/1586745794/eudp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/eudp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/eudp.o.d > ${OBJECTDIR}/_ext/1586745794/eudp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/eudp.o.tmp ${OBJECTDIR}/_ext/1586745794/eudp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/eudp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/eudp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/eudp.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/eudp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/gpfunc.o: ../src/tcpip_bsd/gpfunc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok ${OBJECTDIR}/_ext/1586745794/gpfunc.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d -o ${OBJECTDIR}/_ext/1586745794/gpfunc.o ../src/tcpip_bsd/gpfunc.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d -o ${OBJECTDIR}/_ext/1586745794/gpfunc.o ../src/tcpip_bsd/gpfunc.c   > ${OBJECTDIR}/_ext/1586745794/gpfunc.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d > ${OBJECTDIR}/_ext/1586745794/gpfunc.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.tmp ${OBJECTDIR}/_ext/1586745794/gpfunc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/gpfunc.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/gpfunc.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/gpfunc.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/os.o: ../src/os.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.ok ${OBJECTDIR}/_ext/1360937237/os.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/os.o.d -o ${OBJECTDIR}/_ext/1360937237/os.o ../src/os.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/os.o.d -o ${OBJECTDIR}/_ext/1360937237/os.o ../src/os.c   > ${OBJECTDIR}/_ext/1360937237/os.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/os.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/os.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/os.o.d > ${OBJECTDIR}/_ext/1360937237/os.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/os.o.tmp ${OBJECTDIR}/_ext/1360937237/os.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/os.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/os.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/os.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/os.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/320086670/nat_dp83848c.o: ../src/tcpip_bsd/eth_phy/nat_dp83848c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/320086670 
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d -o ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o ../src/tcpip_bsd/eth_phy/nat_dp83848c.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d -o ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o ../src/tcpip_bsd/eth_phy/nat_dp83848c.c   > ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d > ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.tmp
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	@${CP} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.tmp ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.d 
	@${RM} ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.ok; else cat ${OBJECTDIR}/_ext/320086670/nat_dp83848c.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/network.o: ../src/network.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.ok ${OBJECTDIR}/_ext/1360937237/network.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/network.o.d -o ${OBJECTDIR}/_ext/1360937237/network.o ../src/network.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/network.o.d -o ${OBJECTDIR}/_ext/1360937237/network.o ../src/network.c   > ${OBJECTDIR}/_ext/1360937237/network.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/network.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/network.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/network.o.d > ${OBJECTDIR}/_ext/1360937237/network.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/network.o.tmp ${OBJECTDIR}/_ext/1360937237/network.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/network.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/network.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/network.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/network.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/route.o: ../src/tcpip_bsd/route.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.ok ${OBJECTDIR}/_ext/1586745794/route.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/route.o.d -o ${OBJECTDIR}/_ext/1586745794/route.o ../src/tcpip_bsd/route.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/route.o.d -o ${OBJECTDIR}/_ext/1586745794/route.o ../src/tcpip_bsd/route.c   > ${OBJECTDIR}/_ext/1586745794/route.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/route.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/route.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/route.o.d > ${OBJECTDIR}/_ext/1586745794/route.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/route.o.tmp ${OBJECTDIR}/_ext/1586745794/route.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/route.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/route.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/route.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/route.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/buff_mgr.o: ../src/tcpip_bsd/buff_mgr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/buff_mgr.o ../src/tcpip_bsd/buff_mgr.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d -o ${OBJECTDIR}/_ext/1586745794/buff_mgr.o ../src/tcpip_bsd/buff_mgr.c   > ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d > ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.tmp ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/buff_mgr.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/enc28j60.o: ../src/tcpip_bsd/enc28j60.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok ${OBJECTDIR}/_ext/1586745794/enc28j60.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d -o ${OBJECTDIR}/_ext/1586745794/enc28j60.o ../src/tcpip_bsd/enc28j60.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d -o ${OBJECTDIR}/_ext/1586745794/enc28j60.o ../src/tcpip_bsd/enc28j60.c   > ${OBJECTDIR}/_ext/1586745794/enc28j60.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d > ${OBJECTDIR}/_ext/1586745794/enc28j60.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.tmp ${OBJECTDIR}/_ext/1586745794/enc28j60.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/enc28j60.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/enc28j60.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/enc28j60.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o: ../src/pic32mx_cfg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d -o ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o ../src/pic32mx_cfg.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d -o ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o ../src/pic32mx_cfg.c   > ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d > ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.tmp ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/pic32mx_cfg.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1958230400/dhcp.o: ../src/tcpip_bsd/bsd_dhcp_client/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1958230400 
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok ${OBJECTDIR}/_ext/1958230400/dhcp.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1958230400/dhcp.o.d -o ${OBJECTDIR}/_ext/1958230400/dhcp.o ../src/tcpip_bsd/bsd_dhcp_client/dhcp.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1958230400/dhcp.o.d -o ${OBJECTDIR}/_ext/1958230400/dhcp.o ../src/tcpip_bsd/bsd_dhcp_client/dhcp.c   > ${OBJECTDIR}/_ext/1958230400/dhcp.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1958230400/dhcp.o.d > ${OBJECTDIR}/_ext/1958230400/dhcp.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	@${CP} ${OBJECTDIR}/_ext/1958230400/dhcp.o.tmp ${OBJECTDIR}/_ext/1958230400/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1958230400/dhcp.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1958230400/dhcp.o.ok; else cat ${OBJECTDIR}/_ext/1958230400/dhcp.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/firmware.o: ../src/firmware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.ok ${OBJECTDIR}/_ext/1360937237/firmware.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/firmware.o.d -o ${OBJECTDIR}/_ext/1360937237/firmware.o ../src/firmware.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/firmware.o.d -o ${OBJECTDIR}/_ext/1360937237/firmware.o ../src/firmware.c   > ${OBJECTDIR}/_ext/1360937237/firmware.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/firmware.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/firmware.o.d > ${OBJECTDIR}/_ext/1360937237/firmware.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/firmware.o.tmp ${OBJECTDIR}/_ext/1360937237/firmware.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/firmware.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/firmware.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/firmware.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/firmware.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/pkt_queue.o: ../src/tcpip_bsd/pkt_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d -o ${OBJECTDIR}/_ext/1586745794/pkt_queue.o ../src/tcpip_bsd/pkt_queue.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d -o ${OBJECTDIR}/_ext/1586745794/pkt_queue.o ../src/tcpip_bsd/pkt_queue.c   > ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d > ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.tmp ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/pkt_queue.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/emac.o: ../src/tcpip_bsd/emac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.ok ${OBJECTDIR}/_ext/1586745794/emac.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/emac.o.d -o ${OBJECTDIR}/_ext/1586745794/emac.o ../src/tcpip_bsd/emac.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/emac.o.d -o ${OBJECTDIR}/_ext/1586745794/emac.o ../src/tcpip_bsd/emac.c   > ${OBJECTDIR}/_ext/1586745794/emac.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/emac.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/emac.o.d > ${OBJECTDIR}/_ext/1586745794/emac.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/emac.o.tmp ${OBJECTDIR}/_ext/1586745794/emac.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/emac.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/emac.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/emac.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/emac.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/tcpip_events.o: ../src/tcpip_bsd/tcpip_events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d -o ${OBJECTDIR}/_ext/1586745794/tcpip_events.o ../src/tcpip_bsd/tcpip_events.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d -o ${OBJECTDIR}/_ext/1586745794/tcpip_events.o ../src/tcpip_bsd/tcpip_events.c   > ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d > ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.tmp ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/tcpip_events.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.ok ${OBJECTDIR}/_ext/1360937237/main.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/main.o.d -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1360937237/main.o.d -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c   > ${OBJECTDIR}/_ext/1360937237/main.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1360937237/main.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1360937237/main.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1360937237/main.o.d > ${OBJECTDIR}/_ext/1360937237/main.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${CP} ${OBJECTDIR}/_ext/1360937237/main.o.tmp ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1360937237/main.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1360937237/main.o.ok; else cat ${OBJECTDIR}/_ext/1360937237/main.o.err; exit 1; fi
	
${OBJECTDIR}/_ext/1586745794/ether.o: ../src/tcpip_bsd/ether.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1586745794 
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.ok ${OBJECTDIR}/_ext/1586745794/ether.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/ether.o.d -o ${OBJECTDIR}/_ext/1586745794/ether.o ../src/tcpip_bsd/ether.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -DETH_STARTER_KIT -I"../include" -I"../../../include" -MMD -MF ${OBJECTDIR}/_ext/1586745794/ether.o.d -o ${OBJECTDIR}/_ext/1586745794/ether.o ../src/tcpip_bsd/ether.c   > ${OBJECTDIR}/_ext/1586745794/ether.o.err 2>&1  ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1586745794/ether.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1586745794/ether.o.d > ${OBJECTDIR}/_ext/1586745794/ether.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	@${CP} ${OBJECTDIR}/_ext/1586745794/ether.o.tmp ${OBJECTDIR}/_ext/1586745794/ether.o.d 
	@${RM} ${OBJECTDIR}/_ext/1586745794/ether.o.tmp
endif
	@if [ -f ${OBJECTDIR}/_ext/1586745794/ether.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1586745794/ether.o.ok; else cat ${OBJECTDIR}/_ext/1586745794/ether.o.err; exit 1; fi
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PIC32MXSK=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf ${OBJECTFILES}   ../lib/libidigi.a     -Wl,--defsym=__MPLAB_BUILD=1,--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PIC32MXSK=1,--defsym=_min_heap_size=49000,--defsym=_min_stack_size=1024,--no-check-sections,--gc-sections 
else
dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf ${OBJECTFILES}   ../lib/libidigi.a     -Wl,--defsym=__MPLAB_BUILD=1,--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=49000,--defsym=_min_stack_size=1024,--no-check-sections,--gc-sections
	${MP_CC_DIR}/pic32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/ethstarterkit.X.${IMAGE_TYPE}.elf  
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
