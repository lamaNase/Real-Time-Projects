# MPLAB IDE generated this makefile for use with GNU make.
# Project: main.mcp
# Date: Wed Jul 10 18:00:04 2024

AS = MPASMWIN.exe
CC = 
LD = mplink.exe
AR = mplib.exe
RM = rm

main.cof : main.o
	$(CC) /p16F877A "main.o" /u_DEBUG /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /o"main.cof" /M"main.map" /W /x

main.o : main.asm ../../../../../Program\ Files\ (x86)/Microchip/MPASM\ Suite/P16F877A.INC LCDIS.INC
	$(AS) /q /p16F877A "main.asm" /l"main.lst" /e"main.err" /d__DEBUG=1

clean : 
	$(CC) "main.o" "main.hex" "main.err" "main.lst" "main.cof"

