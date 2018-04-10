#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../translations/BPv3_en_US.s ../1wire.c ../AUXpin.c ../HD44780.c ../I2C.c ../SPI.c ../SUMP.c ../UART.c ../base.c ../baseIO.c ../baseUI.c ../basic.c ../binIO.c ../binIOhelpers.c ../binwire.c ../bitbang.c ../busPirateCore.c ../dio.c ../jtag/ports.c ../1wire_lib.c ../main.c ../pc_at_keyboard.c ../pic.c ../procMenu.c ../raw2wire.c ../raw3wire.c ../selftest.c ../uart2io.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o ${OBJECTDIR}/_ext/1472/1wire.o ${OBJECTDIR}/_ext/1472/AUXpin.o ${OBJECTDIR}/_ext/1472/HD44780.o ${OBJECTDIR}/_ext/1472/I2C.o ${OBJECTDIR}/_ext/1472/SPI.o ${OBJECTDIR}/_ext/1472/SUMP.o ${OBJECTDIR}/_ext/1472/UART.o ${OBJECTDIR}/_ext/1472/base.o ${OBJECTDIR}/_ext/1472/baseIO.o ${OBJECTDIR}/_ext/1472/baseUI.o ${OBJECTDIR}/_ext/1472/basic.o ${OBJECTDIR}/_ext/1472/binIO.o ${OBJECTDIR}/_ext/1472/binIOhelpers.o ${OBJECTDIR}/_ext/1472/binwire.o ${OBJECTDIR}/_ext/1472/bitbang.o ${OBJECTDIR}/_ext/1472/busPirateCore.o ${OBJECTDIR}/_ext/1472/dio.o ${OBJECTDIR}/_ext/760884769/ports.o ${OBJECTDIR}/_ext/1472/1wire_lib.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o ${OBJECTDIR}/_ext/1472/pic.o ${OBJECTDIR}/_ext/1472/procMenu.o ${OBJECTDIR}/_ext/1472/raw2wire.o ${OBJECTDIR}/_ext/1472/raw3wire.o ${OBJECTDIR}/_ext/1472/selftest.o ${OBJECTDIR}/_ext/1472/uart2io.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o.d ${OBJECTDIR}/_ext/1472/1wire.o.d ${OBJECTDIR}/_ext/1472/AUXpin.o.d ${OBJECTDIR}/_ext/1472/HD44780.o.d ${OBJECTDIR}/_ext/1472/I2C.o.d ${OBJECTDIR}/_ext/1472/SPI.o.d ${OBJECTDIR}/_ext/1472/SUMP.o.d ${OBJECTDIR}/_ext/1472/UART.o.d ${OBJECTDIR}/_ext/1472/base.o.d ${OBJECTDIR}/_ext/1472/baseIO.o.d ${OBJECTDIR}/_ext/1472/baseUI.o.d ${OBJECTDIR}/_ext/1472/basic.o.d ${OBJECTDIR}/_ext/1472/binIO.o.d ${OBJECTDIR}/_ext/1472/binIOhelpers.o.d ${OBJECTDIR}/_ext/1472/binwire.o.d ${OBJECTDIR}/_ext/1472/bitbang.o.d ${OBJECTDIR}/_ext/1472/busPirateCore.o.d ${OBJECTDIR}/_ext/1472/dio.o.d ${OBJECTDIR}/_ext/760884769/ports.o.d ${OBJECTDIR}/_ext/1472/1wire_lib.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o.d ${OBJECTDIR}/_ext/1472/pic.o.d ${OBJECTDIR}/_ext/1472/procMenu.o.d ${OBJECTDIR}/_ext/1472/raw2wire.o.d ${OBJECTDIR}/_ext/1472/raw3wire.o.d ${OBJECTDIR}/_ext/1472/selftest.o.d ${OBJECTDIR}/_ext/1472/uart2io.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o ${OBJECTDIR}/_ext/1472/1wire.o ${OBJECTDIR}/_ext/1472/AUXpin.o ${OBJECTDIR}/_ext/1472/HD44780.o ${OBJECTDIR}/_ext/1472/I2C.o ${OBJECTDIR}/_ext/1472/SPI.o ${OBJECTDIR}/_ext/1472/SUMP.o ${OBJECTDIR}/_ext/1472/UART.o ${OBJECTDIR}/_ext/1472/base.o ${OBJECTDIR}/_ext/1472/baseIO.o ${OBJECTDIR}/_ext/1472/baseUI.o ${OBJECTDIR}/_ext/1472/basic.o ${OBJECTDIR}/_ext/1472/binIO.o ${OBJECTDIR}/_ext/1472/binIOhelpers.o ${OBJECTDIR}/_ext/1472/binwire.o ${OBJECTDIR}/_ext/1472/bitbang.o ${OBJECTDIR}/_ext/1472/busPirateCore.o ${OBJECTDIR}/_ext/1472/dio.o ${OBJECTDIR}/_ext/760884769/ports.o ${OBJECTDIR}/_ext/1472/1wire_lib.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o ${OBJECTDIR}/_ext/1472/pic.o ${OBJECTDIR}/_ext/1472/procMenu.o ${OBJECTDIR}/_ext/1472/raw2wire.o ${OBJECTDIR}/_ext/1472/raw3wire.o ${OBJECTDIR}/_ext/1472/selftest.o ${OBJECTDIR}/_ext/1472/uart2io.o

# Source Files
SOURCEFILES=../translations/BPv3_en_US.s ../1wire.c ../AUXpin.c ../HD44780.c ../I2C.c ../SPI.c ../SUMP.c ../UART.c ../base.c ../baseIO.c ../baseUI.c ../basic.c ../binIO.c ../binIOhelpers.c ../binwire.c ../bitbang.c ../busPirateCore.c ../dio.c ../jtag/ports.c ../1wire_lib.c ../main.c ../pc_at_keyboard.c ../pic.c ../procMenu.c ../raw2wire.c ../raw3wire.c ../selftest.c ../uart2io.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ64GA002
MP_LINKER_FILE_OPTION=,--script=../p24FJ64GA002.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/1wire.o: ../1wire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../1wire.c  -o ${OBJECTDIR}/_ext/1472/1wire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/1wire.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/1wire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/AUXpin.o: ../AUXpin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/AUXpin.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AUXpin.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../AUXpin.c  -o ${OBJECTDIR}/_ext/1472/AUXpin.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/AUXpin.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AUXpin.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/HD44780.o: ../HD44780.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/HD44780.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/HD44780.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../HD44780.c  -o ${OBJECTDIR}/_ext/1472/HD44780.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/HD44780.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/HD44780.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/I2C.o: ../I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../I2C.c  -o ${OBJECTDIR}/_ext/1472/I2C.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/I2C.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/I2C.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/SPI.o: ../SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/SPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/SPI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../SPI.c  -o ${OBJECTDIR}/_ext/1472/SPI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/SPI.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/SPI.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/SUMP.o: ../SUMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/SUMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/SUMP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../SUMP.c  -o ${OBJECTDIR}/_ext/1472/SUMP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/SUMP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/SUMP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/UART.o: ../UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/UART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/UART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../UART.c  -o ${OBJECTDIR}/_ext/1472/UART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/UART.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/UART.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/base.o: ../base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/base.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../base.c  -o ${OBJECTDIR}/_ext/1472/base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/base.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/baseIO.o: ../baseIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/baseIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/baseIO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../baseIO.c  -o ${OBJECTDIR}/_ext/1472/baseIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/baseIO.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/baseIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/baseUI.o: ../baseUI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/baseUI.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/baseUI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../baseUI.c  -o ${OBJECTDIR}/_ext/1472/baseUI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/baseUI.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/baseUI.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/basic.o: ../basic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/basic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/basic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../basic.c  -o ${OBJECTDIR}/_ext/1472/basic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/basic.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/basic.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/binIO.o: ../binIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/binIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/binIO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../binIO.c  -o ${OBJECTDIR}/_ext/1472/binIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/binIO.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/binIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/binIOhelpers.o: ../binIOhelpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/binIOhelpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/binIOhelpers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../binIOhelpers.c  -o ${OBJECTDIR}/_ext/1472/binIOhelpers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/binIOhelpers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/binIOhelpers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/binwire.o: ../binwire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/binwire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/binwire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../binwire.c  -o ${OBJECTDIR}/_ext/1472/binwire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/binwire.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/binwire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/bitbang.o: ../bitbang.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/bitbang.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/bitbang.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../bitbang.c  -o ${OBJECTDIR}/_ext/1472/bitbang.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/bitbang.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/bitbang.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/busPirateCore.o: ../busPirateCore.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/busPirateCore.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/busPirateCore.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../busPirateCore.c  -o ${OBJECTDIR}/_ext/1472/busPirateCore.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/busPirateCore.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/busPirateCore.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/dio.o: ../dio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/dio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/dio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../dio.c  -o ${OBJECTDIR}/_ext/1472/dio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/dio.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/dio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/760884769/ports.o: ../jtag/ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760884769" 
	@${RM} ${OBJECTDIR}/_ext/760884769/ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/760884769/ports.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../jtag/ports.c  -o ${OBJECTDIR}/_ext/760884769/ports.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760884769/ports.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/760884769/ports.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/1wire_lib.o: ../1wire_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire_lib.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../1wire_lib.c  -o ${OBJECTDIR}/_ext/1472/1wire_lib.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/1wire_lib.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/1wire_lib.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pc_at_keyboard.o: ../pc_at_keyboard.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pc_at_keyboard.c  -o ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pc_at_keyboard.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pc_at_keyboard.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pic.o: ../pic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/pic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pic.c  -o ${OBJECTDIR}/_ext/1472/pic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pic.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pic.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/procMenu.o: ../procMenu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/procMenu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/procMenu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../procMenu.c  -o ${OBJECTDIR}/_ext/1472/procMenu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/procMenu.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/procMenu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/raw2wire.o: ../raw2wire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/raw2wire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/raw2wire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../raw2wire.c  -o ${OBJECTDIR}/_ext/1472/raw2wire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/raw2wire.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/raw2wire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/raw3wire.o: ../raw3wire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/raw3wire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/raw3wire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../raw3wire.c  -o ${OBJECTDIR}/_ext/1472/raw3wire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/raw3wire.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/raw3wire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/selftest.o: ../selftest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/selftest.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/selftest.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../selftest.c  -o ${OBJECTDIR}/_ext/1472/selftest.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/selftest.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/selftest.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/uart2io.o: ../uart2io.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/uart2io.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/uart2io.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../uart2io.c  -o ${OBJECTDIR}/_ext/1472/uart2io.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/uart2io.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1    -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/uart2io.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/1wire.o: ../1wire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../1wire.c  -o ${OBJECTDIR}/_ext/1472/1wire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/1wire.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/1wire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/AUXpin.o: ../AUXpin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/AUXpin.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AUXpin.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../AUXpin.c  -o ${OBJECTDIR}/_ext/1472/AUXpin.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/AUXpin.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AUXpin.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/HD44780.o: ../HD44780.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/HD44780.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/HD44780.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../HD44780.c  -o ${OBJECTDIR}/_ext/1472/HD44780.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/HD44780.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/HD44780.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/I2C.o: ../I2C.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/I2C.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../I2C.c  -o ${OBJECTDIR}/_ext/1472/I2C.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/I2C.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/I2C.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/SPI.o: ../SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/SPI.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/SPI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../SPI.c  -o ${OBJECTDIR}/_ext/1472/SPI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/SPI.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/SPI.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/SUMP.o: ../SUMP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/SUMP.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/SUMP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../SUMP.c  -o ${OBJECTDIR}/_ext/1472/SUMP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/SUMP.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/SUMP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/UART.o: ../UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/UART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/UART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../UART.c  -o ${OBJECTDIR}/_ext/1472/UART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/UART.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/UART.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/base.o: ../base.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/base.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/base.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../base.c  -o ${OBJECTDIR}/_ext/1472/base.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/base.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/base.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/baseIO.o: ../baseIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/baseIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/baseIO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../baseIO.c  -o ${OBJECTDIR}/_ext/1472/baseIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/baseIO.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/baseIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/baseUI.o: ../baseUI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/baseUI.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/baseUI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../baseUI.c  -o ${OBJECTDIR}/_ext/1472/baseUI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/baseUI.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/baseUI.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/basic.o: ../basic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/basic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/basic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../basic.c  -o ${OBJECTDIR}/_ext/1472/basic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/basic.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/basic.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/binIO.o: ../binIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/binIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/binIO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../binIO.c  -o ${OBJECTDIR}/_ext/1472/binIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/binIO.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/binIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/binIOhelpers.o: ../binIOhelpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/binIOhelpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/binIOhelpers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../binIOhelpers.c  -o ${OBJECTDIR}/_ext/1472/binIOhelpers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/binIOhelpers.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/binIOhelpers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/binwire.o: ../binwire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/binwire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/binwire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../binwire.c  -o ${OBJECTDIR}/_ext/1472/binwire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/binwire.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/binwire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/bitbang.o: ../bitbang.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/bitbang.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/bitbang.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../bitbang.c  -o ${OBJECTDIR}/_ext/1472/bitbang.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/bitbang.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/bitbang.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/busPirateCore.o: ../busPirateCore.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/busPirateCore.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/busPirateCore.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../busPirateCore.c  -o ${OBJECTDIR}/_ext/1472/busPirateCore.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/busPirateCore.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/busPirateCore.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/dio.o: ../dio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/dio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/dio.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../dio.c  -o ${OBJECTDIR}/_ext/1472/dio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/dio.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/dio.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/760884769/ports.o: ../jtag/ports.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760884769" 
	@${RM} ${OBJECTDIR}/_ext/760884769/ports.o.d 
	@${RM} ${OBJECTDIR}/_ext/760884769/ports.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../jtag/ports.c  -o ${OBJECTDIR}/_ext/760884769/ports.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760884769/ports.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/760884769/ports.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/1wire_lib.o: ../1wire_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/1wire_lib.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../1wire_lib.c  -o ${OBJECTDIR}/_ext/1472/1wire_lib.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/1wire_lib.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/1wire_lib.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pc_at_keyboard.o: ../pc_at_keyboard.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pc_at_keyboard.c  -o ${OBJECTDIR}/_ext/1472/pc_at_keyboard.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pc_at_keyboard.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pc_at_keyboard.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pic.o: ../pic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/pic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pic.c  -o ${OBJECTDIR}/_ext/1472/pic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pic.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pic.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/procMenu.o: ../procMenu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/procMenu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/procMenu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../procMenu.c  -o ${OBJECTDIR}/_ext/1472/procMenu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/procMenu.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/procMenu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/raw2wire.o: ../raw2wire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/raw2wire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/raw2wire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../raw2wire.c  -o ${OBJECTDIR}/_ext/1472/raw2wire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/raw2wire.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/raw2wire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/raw3wire.o: ../raw3wire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/raw3wire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/raw3wire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../raw3wire.c  -o ${OBJECTDIR}/_ext/1472/raw3wire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/raw3wire.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/raw3wire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/selftest.o: ../selftest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/selftest.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/selftest.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../selftest.c  -o ${OBJECTDIR}/_ext/1472/selftest.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/selftest.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/selftest.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/uart2io.o: ../uart2io.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/uart2io.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/uart2io.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../uart2io.c  -o ${OBJECTDIR}/_ext/1472/uart2io.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/uart2io.o.d"        -g -omf=elf -no-legacy-libc  -mlarge-code -mlarge-data -O0 -I"../../microchip/include" -I".." -I"../translations" -msmart-io=1 -Werror -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/uart2io.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o: ../translations/BPv3_en_US.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1011476433" 
	@${RM} ${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../translations/BPv3_en_US.s  -o ${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -omf=elf -no-legacy-libc  -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o: ../translations/BPv3_en_US.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1011476433" 
	@${RM} ${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o.d 
	@${RM} ${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../translations/BPv3_en_US.s  -o ${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -no-legacy-libc  -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1011476433/BPv3_en_US.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../p24FJ64GA002.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -omf=elf -no-legacy-libc      -Wl,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1,$(MP_LINKER_FILE_OPTION),--no-check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="../../../../Program Files/Microchip/MPLAB C30/lib",--library-path="..",--no-force-link,--smart-io,-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../p24FJ64GA002.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -no-legacy-libc  -Wl,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--no-check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="../../../../Program Files/Microchip/MPLAB C30/lib",--library-path="..",--no-force-link,--smart-io,-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/busPirate.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
