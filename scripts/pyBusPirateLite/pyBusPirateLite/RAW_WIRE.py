#!/usr/bin/env python
# encoding: utf-8

# This file may conflict with rawwire.py file written by Sean Nelson, I didn't
# see it at his git repository but it was on the code.google.com bus pirate one
# it's the same written in other way, a few more classes, and a bit less errorless.

# In the meantime my examples will be using this, and maybe later I will switch it up.

from .BitBang import *

# 011000xx – Set speed, 3=~400kHz, 2=~100kHz, 1=~50kHz, 0=~5kHz
class RAW_WIRESpeed:
	_5KHZ = 0b000
	_50KHZ = 0b001
	_100KHZ = 0b010
	_400KHZ = 0b011

# 1000wxyz – Config, w=HiZ/3.3v, x=2/3wire, y=msb/lsb, z=not used

class RAW_WIRECfg:
	OUT_TYPE = 0x8
	WIRES = 0x4
	BIT_ORDER = 0x2
	NOT_USED = 0x1

class RAW_WIRE_OUT_TYPE:
	HIZ = 0
	_3V3 = 1

class RAW_WIRE_WIRES_TYPE:
	TWO = 0
	THREE = 1
	
class RAW_WIRE_BIT_ORDER_TYPE:
	MSB = 0
	LSB = 1
	
class RAW_WIRE_COMMANDS:
	RESET 		= "\x00"
	VERSION 	= "\x01"
	I2C_START 	= "\x02"
	I2C_STOP	= "\x03"
	CS_LOW		= "\x04"
	CS_HIGH		= "\x05"
	READ_BYTE	= "\x06"
	READ_BIT	= "\x07"
	PEEK		= "\x08"
	CLK_TICK	= "\x09"
	CLK_LOW		= "\x0A"
	CLK_HIGH	= "\x0B"
	DATA_LOW	= "\x0C"
	DATA_HIGH	= "\x0D"
	BULK_TRANS	= "\x10" # implemented as general commands on BitBang.py
	BULK_CLK	= "\x20"
	CFG_PERIPHERALS = "\x40"
	SET_SPEED	= "\x60"
	CFG_MODE	= "\x80"
	

class RAW_WIRE(BBIO):
	bulk_read = None
	def __init__(self, port, speed):
		BBIO.__init__(self, port, speed)
		
	#def enter_mode(self):
	#	if self.BBmode() && self.enter_raw_wire(): return True
	#	else: return False
	#	    if not rw.BBmode():
		
	def command(self, command, response_length, timeout=0.1):
		self.port.write(command)
		self.timeout(timeout)
		return self.response(response_length, True)
		
	# 00000000 – Enter raw bitbang mode, reset to raw bitbang mode
	# implement it?
	
	# 00000001 – Mode version string (RAW1)
	
	def version(self):
		return self.command(RAW_WIRE_COMMANDS.VERSION,4)
		
	# 0000001x – I2C-style start (0) / stop (1) bit
	
	def I2C_Start(self):
		return self.command(RAW_WIRE_COMMANDS.I2C_START,1)
	
	def I2C_Stop(self):
		return self.command(RAW_WIRE_COMMANDS.I2C_STOP,1)
		
	# 0000010x- CS low (0) / high (1)

	def CS_Low(self):
		return self.command(RAW_WIRE_COMMANDS.CS_LOW,1)

	def CS_High(self):
		return self.command(RAW_WIRE_COMMANDS.CS_HIGH,1)
		
	# 00000110 – Read byte
	# Ummm: Reads a byte from the bus, returns the byte. Writes 0xff to bus in 3-wire mode.
	# writes 0xff in 3-wire mode, in order to maintain the data line high?
	
	def read_byte(self):
		return self.command(RAW_WIRE_COMMANDS.READ_BYTE,1)
	
	# 00000111 – Read bit	
	def read_bit(self):
		return self.command(RAW_WIRE_COMMANDS.READ_BIT,1)
		
	# 00001000 – Peek at input pin
	
	def peek(self):
		return self.command(RAW_WIRE_COMMANDS.PEEK,1)
	
	# 00001001 – Clock tick
	
	def clk_tick(self):
		return self.command(RAW_WIRE_COMMANDS.CLK_TICK,1)
			
	# 0000101x – Clock low (0) / high (1)

	def clk_low(self):
		return self.command(RAW_WIRE_COMMANDS.CLK_LOW,1)
	
	def clk_high(self):
		return self.command(RAW_WIRE_COMMANDS.CLK_HIGH,1)
		
	# 0000110x – Data low (0) / high (1)

	def data_low(self):
		return self.command(RAW_WIRE_COMMANDS.DATA_LOW,1)
		
	def data_high(self):
		return self.command(RAW_WIRE_COMMANDS.DATA_HIGH,1)
		
	# 0001xxxx – Bulk transfer, send 1-16 bytes (0=1byte!)
	#def bulk_trans(self, byte_count=1, byte_string=None):
	#
	# implemented on BitBang
	
	# 0010xxxx – Bulk clock ticks, send 1-16 ticks
	
	def bulk_clk(self, clk_count=0): # 0 == 1 clock
		return self.command( chr(0x20 | clk_count), 1 );
	
	# 0100wxyz – Configure peripherals, w=power, x=pullups, y=AUX, z=CS
	
	# def cfg_pins(self, pins=0): at BitBang upper class
	
	# 0110000x – Set speed, low (0=~5kHz) / high (1=~50kHz) changed in v4.2
	# 011000xx – Set speed, 3=~400kHz, 2=~100kHz, 1=~50kHz, 0=~5kHz
	
	#def set_speed(self, bus_speed=0): at BitBang upper class
	
	# 1000wxyz – Configure mode, w=output type, x=2/3wire, y=msb/lsb, z=n/a
	
	def cfg_raw_wire(self, raw_wire_cfg):
		return self.command( chr(0x80 | raw_wire_cfg ), 1)


