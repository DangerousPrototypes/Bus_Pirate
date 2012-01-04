#!/usr/bin/env python
# encoding: utf-8
"""
Created by Ondrej Caletka on 2010-11-13.
Copyright 2010 Ondrej Caletka <ondrej.caletka@gmail.com>
Based on the i2c demo from Peter Huewe.

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
from pyBusPirateLite.I2Chigh import *

class DS_Config:
    _9BIT = 0x00
    _10BIT = 0x04
    _11BIT = 0x08
    _12BIT = 0x0C
    ONESHOT = 0x01
    POL = 0x02
    NVB = 0x10
    TLF = 0x20
    TLH = 0x40
    DONE = 0x80

class DS1631:
    """ DS1631 thermometer intraface. Uses I2Chigh class for communication with device. """
    def __init__(self, i2c, address=0x48):
        self.i2c = i2c;
        self.address = address;
        self.soft_por();

    def get_temp_reg(self, reg):
        temp = self.i2c.get_word(self.address, reg);
        if (temp & 0x0f):
            raise ValueError, 'Invalid value received!';
        if temp < 32768:
            return temp/256.0;
        else:
            return (temp-65536)/256.0;

    def set_temp_reg(self, reg, value):
        if value >= 0:
            rawval = int(round(256*value)) & 0xfff0;
        else:
            rawval = (65536 + int(round(256*value))) & 0xfff0;
        self.i2c.set_word(self.address, reg, rawval);

    def get_temp(self):
        return self.get_temp_reg(0xAA);
    
    def get_tl(self):
        return self.get_temp_reg(0xA1);

    def set_tl(self, value):
        self.set_temp_reg(0xA1, value);

    def get_th(self):
        return self.get_temp_reg(0xA2);

    def set_th(self, value):
        self.set_temp_reg(0xA2, value);

    def get_config(self):
        return self.i2c.get_byte(self.address, 0xAC);

    def set_config(self, config):
        self.i2c.set_byte(self.address, 0xAC, config);

    def start_convert(self):
        self.i2c.command(self.address, 0x51);

    def stop_convert(self):
        self.i2c.command(self.address, 0x22);

    def soft_por(self):
        self.i2c.command(self.address, 0x54);

    def is_done(self):
        return self.get_config() & DS_Config.DONE;



if __name__ == '__main__':
    try:
        # Serial timeout five seconds for debugging mistakes in I2C class
        i2c = I2Chigh("/dev/ttyUSB0", 115200, 5) 
    except Exception, e:
        print "Error",e
        sys.exit()

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
    if not i2c.set_speed(I2CSpeed._100KHZ):
        print "Failed to set I2C Speed."
        sys.exit()
    i2c.timeout(0.2)
    
    print "Starting..."

    try:
        ds1 = DS1631(i2c, 0x48);
        ds1.set_config(DS_Config._12BIT | DS_Config.ONESHOT | DS_Config.POL);
    except Exception, e:
        print "Thermometer 1: ",e;
        ds1 = None;
        
    try:
        ds2 = DS1631(i2c, 0x49);
        ds2.set_config(DS_Config._12BIT | DS_Config.ONESHOT | DS_Config.POL);
    except Exception, e:
        print "Thermometer 2: ",e;
        ds2 = None;

    print "Thermometers initialized"

    temp1=0;
    temp2=0;
    for i in range(16):
        print "Round %00d" % (i+1),;
        if (ds1):
            ds1.start_convert();
        if (ds2):
            ds2.start_convert();
        print "started..."
        while not ((not ds1 or ds1.is_done()) and (not ds2 or ds2.is_done())):
            print "In Progress..."
            i2c.timeout(0.2)

        if (ds1):
            tmp=ds1.get_temp();
            print "T1: %f " %(tmp);
            temp1 += tmp/16.0;

        if (ds2):
            tmp=ds2.get_temp();
            print "T2: %f" %(tmp);
            temp2 += tmp/16.0;
        
    print "Final T1: %f T2: %f" %(temp1, temp2);

    print "Reset Bus Pirate to user terminal: "
    if i2c.resetBP():
        print "OK."
    else:
        print "failed."
        sys.exit()
        

