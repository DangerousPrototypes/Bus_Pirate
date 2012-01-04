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
from serial.serialutil import SerialException
import sys, optparse
from pyBusPirateLite.SPI import *

def read_list_data(size):
	data = []
	for i in range(size+1):
		data.append(0)
	return data

def parse_prog_args():
	parser = optparse.OptionParser(usage="%prog [options] filename",
									version="%prog 1.0")

	parser.set_defaults(command="read")

	parser.add_option("-v", "--verbose",
						action="store_true", dest="verbose", default=True,
						help="make lots of noise [default]")
	parser.add_option("-q", "--quiet",
						action="store_false", dest="verbose",
						help="be mute")
	parser.add_option("-r", "--read",
						action="store_const", dest="command", const="read",
						help="read from SPI to file [default]")
	parser.add_option("-w", "--write",
						action="store_const", dest="command", const="write",
						help="write from file to SPI")
	parser.add_option("-e", "--erase",
						action="store_const", dest="command", const="erase",
						help="erase SPI")
	parser.add_option("-i", "--id",
						action="store_const", dest="command", const="id",
						help="print Chip ID")
	parser.add_option("-s", "--size",
	 					dest="flash_size", default=128,
						help="Size of Flashchip in bytes", type="int")
	parser.add_option("-d", "--dev",
	 					dest="dev_name", default="/dev/tty.usbserial-A7004qlY",
						help="The device to connect to", type="string")

	(options, args) = parser.parse_args()

	if options.command == "id":
		return (options, args)
	elif len(args) != 1:
		parser.print_help()
		print options
		sys.exit(1)
	else:
		return (options, args)
	
""" enter binary mode """
if __name__ == '__main__':
	data = ""
	(opt, args)  = parse_prog_args()

	if opt.command == "read":
		f=open(args[0], 'wb')
	elif opt.command == "write":
		f=open(args[0], 'rb')

	try:
		spi = SPI(opt.dev_name, 115200)
	except SerialException as ex:
		print ex
		sys.exit();

	print "Entering binmode: ",
	if spi.BBmode():
		print "OK."
	else:
		print "failed."
		sys.exit()

	print "Entering raw SPI mode: ",
	if spi.enter_SPI():
		print "OK."
	else:
		print "failed."
		sys.exit()
		
	print "Configuring SPI peripherals: ",
	if spi.cfg_pins(PinCfg.POWER | PinCfg.CS | PinCfg.AUX):
		print "OK."
	else:
		print "failed."
		sys.exit()
	print "Configuring SPI speed: ",
	if spi.set_speed(SPISpeed._2_6MHZ):
		print "OK."
	else:
		print "failed."
		sys.exit()
	print "Configuring SPI configuration: ",
	if spi.cfg_spi(SPICfg.CLK_EDGE | SPICfg.OUT_TYPE):
		print "OK."
	else:
		print "failed."
		sys.exit()
	spi.timeout(0.2)

	if opt.command == "read":
		print "Reading EEPROM."
		spi.CS_Low()
		spi.bulk_trans(5, [0xB, 0, 0, 0, 0])
		for i in range((int(opt.flash_size)/16)):
			data = spi.bulk_trans(16, read_list_data(16))
			f.write(data)
		spi.CS_High()

	elif opt.command == "write":
		print "Writing EEPROM."
		spi.CS_Low()
		spi.bulk_trans(4, [0xA, 0, 0, 0])
		for i in range((int(opt.flash_size)/16)):
			spi.bulk_trans(16, None)
		spi.CS_High()

	elif opt.command == "id":
		print "Reading Chip ID: ",
		spi.CS_Low()
		d = spi.bulk_trans(4, [0x9F, 0, 0, 0])
		spi.CS_High()
		for each in d[1:]:
			print "%02X " % ord(each),
		print

	elif opt.command == "erase":
		pass

	print "Reset Bus Pirate to user terminal: ",
	if spi.resetBP():
		print "OK."
	else:
		print "failed."
		sys.exit()
		
