#!/usr/bin/env python
# encoding: utf-8
"""
Created by Sean Nelson on 2009-10-14.
Copyright 2009 Sean Nelson <audiohacked@gmail.com>

This file is part of pyBusPirate.

pyBusPirate is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

pyBusPirate is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pyBusPirate.  If not, see <http://www.gnu.org/licenses/>.
"""

from .BitBang import *

class SPISpeed:
	_30KHZ = 0b000
	_125KHZ = 0b001
	_250KHZ = 0b010
	_1MHZ = 0b011
	_2MHZ = 0b100
	_2_6MHZ = 0b101
	_4MHZ = 0b110
	_8MHZ = 0b111

class SPICfg:
	OUT_TYPE = 0x8
	IDLE = 0x4
	CLK_EDGE = 0x2
	SAMPLE = 0x1

class SPI_OUT_TYPE:
	HIZ = 0
	_3V3 = 1

class SPI(BBIO):
	bulk_read = None
	def __init__(self, port, speed):
		BBIO.__init__(self, port, speed)

	def CS_Low(self):
		self.port.write("\x02")
		self.timeout(0.1)
		return self.response(1, True)

	def CS_High(self):
		self.port.write("\x03")
		self.timeout(0.1)
		return self.response(1, True)

	def low_nibble(self, nibble):
		self.port.write(chr(0x20 | nibble))
		self.timeout(0.1)
		return self.response(1, True)

	def high_nibble(self, nibble):
		self.port.write(chr(0x30 | nibble))
		self.timeout(0.1)
		return self.response(1, True)

	def cfg_spi(self, spi_cfg):
		self.port.write(chr(0x80 | spi_cfg))
		self.timeout(0.1)
		return self.response()

	def read_spi_cfg(self):
		self.port.write("\x90")
		self.timeout(0.1)
		return self.response(1, True)

