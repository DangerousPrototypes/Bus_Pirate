#!/usr/bin/env python
# encoding: utf-8
"""
Created by Peter Huewe on 2009-10-26.
Copyright 2009 Peter Huewe <peterhuewe@gmx.de>
Based on the spi testscript from Sean Nelson

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
import sys
from pyBusPirateLite.I2C import *
""" enter binary mode """


def i2c_write_data(data):
	i2c.send_start_bit()
	i2c.bulk_trans(len(data),data)
	i2c.send_stop_bit()


def i2c_read_bytes(address, numbytes, ret=False):
	data_out=[]
	i2c.send_start_bit()
	i2c.bulk_trans(len(address),address)
	while numbytes > 0:
		if not ret:
			print ord(i2c.read_byte())
		else:
			data_out.append(ord(i2c.read_byte()))
		if numbytes > 1:
			i2c.send_ack()
		numbytes-=1
	i2c.send_nack()
	i2c.send_stop_bit()
	if ret:
		return data_out

if __name__ == '__main__':
	i2c = I2C("/dev/ttyUSB0", 115200)
	print "Entering binmode: ",
	if i2c.BBmode():
		print "OK."
	else:
		print "failed."
		sys.exit()

	print "Entering raw I2C mode: ",
	if i2c.enter_I2C():
		print "OK."
	else:
		print "failed."
		sys.exit()
		
	print "Configuring I2C."
	if not i2c.cfg_pins(I2CPins.POWER | I2CPins.PULLUPS):
		print "Failed to set I2C peripherals."
		sys.exit()
	if not i2c.set_speed(I2CSpeed._50KHZ):
		print "Failed to set I2C Speed."
		sys.exit()
	i2c.timeout(0.2)
	
	print "Reading EEPROM."
	i2c_write_data([0xa0, 0,0, 1, 2,3,4,5,6,7,8,9])
	i2c_write_data([0xa0, 0,0])
	print i2c_read_bytes([0xa1],5)
	
	print "Reset Bus Pirate to user terminal: "
	if i2c.resetBP():
		print "OK."
	else:
		print "failed."
		sys.exit()
		
