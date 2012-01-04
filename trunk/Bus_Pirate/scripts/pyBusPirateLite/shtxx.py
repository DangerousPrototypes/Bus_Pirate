#!/usr/bin/env python
# encoding: utf-8

#based on Microwire.py and hackaday buspirate/sht tutorial
import sys,time

from optparse import OptionParser
from pyBusPirateLite.RAW_WIRE import *

def sht_command(rw, data):
    ##clear shtxx interface
    rw.data_high()
    for i in range(9):
        rw.clk_tick()
    #start condition
    rw.data_high()
    rw.clk_high()
    rw.data_low()
    rw.clk_low()
    rw.clk_high()
    rw.data_high()
    rw.clk_low()
    #command
    rw.bulk_trans(1, [data])
    #read and return bit
    return rw.read_bit()    

def sht_acknowledge(rw):
    #acknowledge
    rw.data_low()
    rw.clk_tick()

def sht_wait_conversion_finished(rw, options):
    while 1:
        mosi_status = (ord(rw.read_pins()) & BBIOPins.MOSI) >> (BBIOPins.MOSI-1)
        if mosi_status:
            if options.verbose:
                print 'waiting...'
        else:
            if options.verbose:
                print 'conversion done'
            time.sleep(0.1)
            break
            
def sht_temperature(rw, options):    
    #soft reset
    status = sht_command(rw, 0b00011110)
    if options.verbose:
        print 'acknowledgment status:', ord(status)
    if not status:
        print "Error resetting SHT"

    #start temperature conversion
    status = sht_command(rw, 0b00000011)
    if options.verbose:
        print 'acknowledgment status:', ord(status)
    if not status:
        print "Error starting temperature conversion SHT"

    sht_wait_conversion_finished(rw, options)
    data = list()
    for i in range(3):
        data.append(ord(rw.read_byte()))
        sht_acknowledge(rw)
    temp_hb, temp_lb ,temp_crc =  data
    #temp_hb = 0x17 #for formula testing
    #temp_lb = 0xcc #for formula testing
    #print (temp_hb<<8)+temp_lb
    temp = -39.7 + 0.01 * ((temp_hb<<8)+temp_lb)

    if options.verbose:
        print 'temp_hb:', temp_hb
        print 'temp_lb:', temp_lb
        print 'temp_crc:', temp_crc
        print 'temp:', temp
    return temp

def sht_humidity(rw, options):    
    #soft reset
    status = sht_command(rw, 0b00011110)
    if options.verbose:
        print 'acknowledgment status:', ord(status)
    if not status:
        print "Error resetting SHT"

    #start humidity conversion
    status = sht_command(rw, 0b00000101)
    if options.verbose:
        print 'acknowledgment status:', ord(status)
    if not status:
        print "Error starting humidity conversion SHT"
    #time.sleep(1)

    sht_wait_conversion_finished(rw, options)
    data = list()
    for i in range(3):
        data.append(ord(rw.read_byte()))
        sht_acknowledge(rw)
    hum_hb, hum_lb ,hum_crc =  data
    #hum_hb = 0x05 #for formula testing
    #hum_lb = 0x80 #for formula testing
    #print (hum_hb<<8)+hum_lb
    hum = -2.0468 + 0.0367*((hum_hb<<8)+hum_lb) + (-0.0000015955*(((hum_hb<<8)+hum_lb)**2))

    if options.verbose:
        print 'hum_hb:', hum_hb
        print 'hum_lb:', hum_lb
        print 'hum_crc:', hum_crc
        print 'hum:', hum
    return hum

def main():
    # First of all parse the command line
    
    parser = OptionParser()

    parser.add_option("-d", "--device", dest="device", help="serial interface where bus pirate is in.[/dev/bus_pirate]", default="/dev/bus_pirate")
    parser.add_option("-t", "--temperature", action="store_true", dest="temperature", help="get temperature from sht.", default=False)
    parser.add_option("-H", "--humidity", dest="humidity", action="store_true", help="get humidity from sht.", default=False)
    parser.add_option("-v", "--verbose", dest="verbose", help="don't be quiet.", action="store_true")
    
    (options,args) = parser.parse_args()
    
    if not (options.temperature or options.humidity):
        parser.print_help()
        exit()      
    
    # Create an instance of the RAW_WIRE class as we are using the BitBang/RAW_WIRE mode
    rw = RAW_WIRE( options.device, 115200 )
    
    if not rw.BBmode():
        print "Can't enter into BitBang mode."
        exit()

    # We have succesfully activated the BitBang Mode, so we continue with
    # the raw-wire mode.
    if not rw.enter_rawwire():
        print "Can't enable the raw-wire mode."
        exit()
        
    # Now we have raw-wire mode enabled, so first configure peripherals
    # (Power, PullUps, AUX, CS)
    
    if not rw.raw_cfg_pins( PinCfg.POWER | PinCfg.PULLUPS):
        print "Error enabling the internal voltage regulators."
        
    # Configure the raw-wire mode
    
    if not rw.cfg_raw_wire( (RAW_WIRECfg.BIT_ORDER & RAW_WIRE_BIT_ORDER_TYPE.MSB) | (RAW_WIRECfg.WIRES & RAW_WIRE_WIRES_TYPE.TWO) | (RAW_WIRECfg.OUT_TYPE & RAW_WIRE_OUT_TYPE.HIZ) ):
        print "Error configuring the raw-wire mode."
    
    # Set raw-wire speed
    
    if not rw.set_speed( RAW_WIRESpeed._5KHZ ):
        print "Error setting raw-wire speed."

    if options.temperature:
        print "Measuring temperature..."
        temperature = sht_temperature(rw, options)
        print "Temperature: %f°C" % temperature

    if options.humidity:
        print "Measuring humidity..."
        humidity = sht_humidity(rw, options)
        print "Humidity: %f%%" % humidity
    
    # Reset the bus pirate
    rw.resetBP();

if __name__ == '__main__':
    
    main()
