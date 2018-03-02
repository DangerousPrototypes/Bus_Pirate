import sys
import serial
import os
import argparse
from colorama import init
from colorama import Fore
from colorama import Style
from colorama import Back
from pprint import pprint
import subprocess

parser = argparse.ArgumentParser(description='Bus Pirate Automated Test Suite v1.0')
parser.add_argument('--port', required=False, default='COM4', help='Bus Pirate serial port name')

args = vars(parser.parse_args())
class bcolors:
	OKGREEN = '\033[95m'
	WARNING = '\033[93m'
	ENDC = '\033[0m'
	
class testSuite:
	def __init__(self, p='COM4', s=115200, t=1):
		self.portConfig={}
		self.portConfig['port']=p
		self.portConfig['speed']=s
		self.portConfig['timeout']=t 
		
	def program(self):
		#this doesn't work because programmer causes bootloader mode
		print(subprocess.call(['C:/Program Files (x86)/Microchip/MPLABX/v3.26/mplab_ipe/ipecmd.exe','-P24FJ64GA002', '-TPPK3', '-M', '-F"c:/buspirate/bpv3-BL44FW510-DUMP.hex"','-OL']))	#,	-P24FJ64GA002 -TPPK3 -M -F"c:\buspirate\bpv3-BL44FW510-DUMP.hex" -W3.3
	
	def startTest(self):
		self.port = serial.Serial(self.portConfig['port'], self.portConfig['speed'], timeout=self.portConfig['timeout'])
		self.port.flushInput()
		#do selftest
		self.port.write("~\n ".encode())
		self.testData = self.port.read(10000).decode().splitlines(0)	

	def endTest(self):
		self.port.write(" ".encode())
		data = self.port.read(10000).decode().splitlines(0)
		if data[0] != 'Found 0 errors.':
			print()
			for result in self.testData:
				if result[-4:]=='FAIL':
					print(Back.RED + result + Style.RESET_ALL)
			print(Back.RED + 'FAIL!!! :(' + Style.RESET_ALL + "\n")
		else:
			print("\n" + Back.GREEN + 'OK :)' + Style.RESET_ALL + "\n")
		self.port.close()
	def timeout(self, timeout=0.1):
		select.select({}, {}, {}, timeout)

init()  #colorama

t=testSuite(args['port'],115200,1)



while True:
	key=input("'ENTER' to start test")
	t.startTest()
	print("\n     Check "+Back.RED + "L" + Back.YELLOW + "E" + Back.RED + "D" + Style.RESET_ALL + "\n")
	key=input("'ENTER' to end test")
	t.endTest()
	#show red/green test result
