#!/usr/bin/env python
# encoding: utf-8
"""
 Bus Pirate Noritake Serial VFD display demo
 Created by Edouard Lafargue on 2010-02-22
 Copyright 2010 Edouard Lafargue <edouard@lafargue.name>

This demo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this demo script.  If not, see <http://www.gnu.org/licenses/>.

"""

import os, sys
import Image
import serial
from pyBusPirateLite.UART import *
from pyBusPirateLite.BitBang import *

# This utility takes a 140x32 GIF black and white image file and
# displays it on a Noritake "Show and Tell" VFD.

# Takes the filename as the single argument, connects to the
# VFD through the Bus Pirate.

# The VFD display is a Noritake GU140x32F-7002, hooked to the bus pirate
# through its serial connector, all with default jumper settings: async serial
# 38400 Baud. The AUX Pin on the BP is connected to the Reset pin so that the display
# can be reset easily.

for infile in sys.argv[1:]:
    f, e = os.path.splitext(infile)
    if e == ".gif":
        try:
            im = Image.open(infile)
            print "Picture size: " , im.size
        except IOError:
            print "cannot open source bitmap", infile
		
# Now parse the bitmap to transform it into a bitstream
# as expected by the display.
# Data is sent by column, one byte per 8 rows of 1 pixel, MSB on top
# So the data size is:
#   bitmapsize = im.size[0]*(im.size[1])//8

print "Converting bitmap..."
picturebitmap = []
# BW GIF pixels have a 0 value for white and 1 value for black.
for col in range (0,im.size[0]):
    for row in range (0,im.size[1]//8):
		pixblock = 0
		for subrow in range (0,8):
			pixblock = pixblock | ( im.getpixel((col,row*8+subrow)) << (7-subrow))
		picturebitmap.append(pixblock)


# Now initialize the display: we want to display a full screen picture, so
# we will clear the screen, place the cursor home.
uart = UART("COM4",115200)
print "Entering binmode: ",
if uart.BBmode():
	print "OK."
else:
	print "failed."
	sys.exit()

print "Entering binary UART mode: ",
if uart.enter_UART():
	print "OK."
else:
	print "failed."
	sys.exit()

print "Reset display (toggle AUX): ",
uart.cfg_pins(0x00)
uart.cfg_pins(PinCfg.POWER | PinCfg.AUX | PinCfg.PULLUPS)
print "OK."
	
print "Setting UART speed to 38400",
if uart.set_speed(UARTSpeed._38400):
	print "OK."
else:
	print "failed"
	sys.exit()

# Last, cross fingers and send the bitmap
# Command is: 1F 28 66 11 xL xH yL yH 01 [bitmap]
# We assume width is < 256, so xH is 0x00. Likewise, yH is always 0x00 on a 140x32 display
initstring = [ 0x0C, 0x1F, 0x28, 0x66, 0x11, im.size[0], 0x00, im.size[1]//8, 0x00, 0x01]
uart.bulk_trans(len(initstring), initstring)

print "send Bitmap"
for i in range((len(picturebitmap)/16)):
	print ".",
	uart.bulk_trans(16, picturebitmap[i*16:i*16+16])

