# MPLAB IDE generated this makefile for use with GNU make.
# Project: bpv4-bootloader.mcp
# Date: Fri Aug 12 10:27:36 2011

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

bpv4-bootloader.hex : bpv4-bootloader.cof
	$(HX) "bpv4-bootloader.cof"

bpv4-bootloader.cof : bootloader.o cdc.o main.o usb_stack.o
	$(CC) -mcpu=24FJ256GB106 "bootloader.o" "cdc.o" "main.o" "usb_stack.o" -o"bpv4-bootloader.cof" -Wl,--script="p24FJ256GB106.gld",--defsym=__MPLAB_BUILD=1,-Map="bpv4-bootloader.map",--report-mem

bootloader.o : bootloader.h cdc.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24FJ256GB106.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24fxxxx.h picusb.h usb_stack.h boot_config.h c:/program\ files/microchip/mplabc30/v3.25/include/string.h c:/program\ files/microchip/mplabc30/v3.25/include/stdlib.h c:/program\ files/microchip/mplabc30/v3.25/include/stdarg.h c:/program\ files/microchip/mplabc30/v3.25/include/stddef.h c:/program\ files/microchip/mplabc30/v3.25/include/stdio.h globals.h bootloader.c
	$(CC) -mcpu=24FJ256GB106 -x c -c "bootloader.c" -o"bootloader.o" -g -Wall

cdc.o : cdc.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24FJ256GB106.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24fxxxx.h picusb.h usb_stack.h boot_config.h c:/program\ files/microchip/mplabc30/v3.25/include/string.h c:/program\ files/microchip/mplabc30/v3.25/include/stdlib.h c:/program\ files/microchip/mplabc30/v3.25/include/stdarg.h c:/program\ files/microchip/mplabc30/v3.25/include/stddef.h c:/program\ files/microchip/mplabc30/v3.25/include/stdio.h globals.h cdc.c
	$(CC) -mcpu=24FJ256GB106 -x c -c "cdc.c" -o"cdc.o" -g -Wall

main.o : usb_lang.h descriptors.h configwords.h cdc.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24FJ256GB106.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24fxxxx.h picusb.h usb_stack.h boot_config.h c:/program\ files/microchip/mplabc30/v3.25/include/string.h c:/program\ files/microchip/mplabc30/v3.25/include/stdlib.h c:/program\ files/microchip/mplabc30/v3.25/include/stdarg.h c:/program\ files/microchip/mplabc30/v3.25/include/stddef.h c:/program\ files/microchip/mplabc30/v3.25/include/stdio.h globals.h main.c
	$(CC) -mcpu=24FJ256GB106 -x c -c "main.c" -o"main.o" -g -Wall

usb_stack.o : cdc.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24FJ256GB106.h c:/program\ files/microchip/mplabc30/v3.25/support/PIC24F/h/p24fxxxx.h picusb.h usb_stack.h boot_config.h c:/program\ files/microchip/mplabc30/v3.25/include/string.h c:/program\ files/microchip/mplabc30/v3.25/include/stdlib.h c:/program\ files/microchip/mplabc30/v3.25/include/stdarg.h c:/program\ files/microchip/mplabc30/v3.25/include/stddef.h c:/program\ files/microchip/mplabc30/v3.25/include/stdio.h globals.h usb_stack.c
	$(CC) -mcpu=24FJ256GB106 -x c -c "usb_stack.c" -o"usb_stack.o" -g -Wall

clean : 
	$(RM) "bootloader.o" "cdc.o" "main.o" "usb_stack.o" "bpv4-bootloader.cof" "bpv4-bootloader.hex"

