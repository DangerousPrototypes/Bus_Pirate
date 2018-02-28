import sys
import json
import serial
import time
import os
import glob
import requests
import argparse
from pprint import pprint

parser = argparse.ArgumentParser(description='Bus Pirate Automated Test Suite v1.0')
parser.add_argument('--port', required=False, default='COM4', help='Bus Pirate serial port name')
parser.add_argument('--test', required=True, help='Test script (folder) to load')
parser.add_argument('--result', required=False, default='result.json', help='Test result file name')
parser.add_argument('--upload', required=False, action='store_true', help='Upload results?')
parser.add_argument('--url', required=False, help='Upload URL')
parser.add_argument('--apikey', required=False, help='Upload API key')

args = vars(parser.parse_args())
#pprint( args)
#if --upload make sure url and api key specified
if args['upload']==True:
	if args['url']==None or args['apikey']==None:
		print('Specify upload URL and API key or remove --upload option')
		sys.exit(2)

class testSuite:
	def __init__(self, p='COM4', s=115200, t=1):
		self.port = serial.Serial(p, s, timeout=t)
		self.port.flushInput()
		self.version={}
	
	def getVersion(self):
		#get bus pirate hardware and firmware version
		self.port.flushInput()
		self.port.write("\n".encode())
		self.port.read(10000)
		self.port.flushInput()
		self.port.write("\n".encode())
		self.port.write("i\n".encode())
		data = self.port.read(10000).decode()
		lines=data.splitlines(0)
		self.lastPrompt=lines[-1];
		#pprint(lines)
		self.version['hardware']=lines[2].rpartition('v')[2]
		self.version['hardwareMajor']=self.version['hardware'].split('.')[0]
		firmwareInfo=lines[3].split()
		self.version['firmware']=firmwareInfo[1].strip('v')
		self.version['commit']=firmwareInfo[2].strip('r')
		#pprint(self.version)
		print('Hardware: ' + self.version['hardware'])
		print('Firmware: ' + self.version['firmware'] + ' commit: '+self.version['commit'] )
		return data

	def importTest(self,test):
		t = json.load(open(test))
		#test if this is a test for our hardware version
		#if no version string, set one
		try: 
			if self.version['hardwareMajor'] not in t['hardware']:
				print("Not for this hardware, skipping")
				return False
		except KeyError: 
			print("Hardware version not defined, using on all versions")
			t['hardware']={}
			t['hardware'].append(self.version['hardwareMajor'])
		#pprint(self.version['hardwareMajor'])	
		#pprint(t['hardware'])	
		
		self.test={}
		try: 
			self.test['device']=t['device']
		except KeyError: 
			print("Missing device ID, using 'unknown'")
			self.test['device']='unknown'

		try: 
			self.test['position']=t['position']
		except KeyError: 
			print("Missing device position, using null")
			self.test['position']=null

			
		if 'test' not in t:
			print("Missing test directives!")
			self.test['test']={}
		else:
			print("Test sections: " + str(len(t['test'])))	
			self.test['test']=t['test']
		
		return True
		
		
	def run(self):
	
		#test output array
		result={}
		
		result['timestamp']={}
		result['timestamp']['start']=time.time()
		result['timestamp']['stop']=time.time()
		result['device']=self.test['device']
		result['position']=self.test['position']
		
		result['test']={}
	
		i=0
		
		#loop tests
		self.port.flushInput()
		for section in self.test['test']:
			#get section name
			if 'name' not in section:
				print("Missing section name, using 'unknown_'"+i)
				#use default section name
				section['name']='unknown_'+i
			else:
				print("Starting section: " + section['name'])
				#add to output array
				
			result['test'][section['name']]=[]
		
			if 'steps' not in section:
				print('Missing test steps, doing nothing!')
				continue;
			else:
				print('Test steps: '+ str(len(section['steps'])))
				
			for step in section['steps']:
				
				#if 'hardware' not in then all else check against this version
				if 'hardware' in step:
					if self.version['hardwareMajor'] not in step['hardware']:
						continue;
				
				if 'commands' not in step:
					print('Missing test commands, doing nothing!')
					continue;
				
				#loop commands
				for command in step['commands']:	
				
					output={}
				
					#write command out
					self.port.write(command.encode());
					output['command']=command
					
					#if 'linefeed' not in then linefeed else check against the linefeed values
					if 'linefeed' not in step: #default is line feed
						self.port.write("\n".encode())
					else:
						if step['linefeed']==True:
							self.port.write("\n".encode())
							
					#save output in array with step/section name/input and output sections
					data = self.port.read(10000).decode()
					print(self.lastPrompt + data)					
					output['result']=self.lastPrompt + data
					
					lines=data.splitlines(0)
					self.lastPrompt=lines[-1]
					
					result['test'][section['name']].append(output)
			i=i+1
					
		result['timestamp']['stop']=time.time()
		r={}
		r[result['device']]=result
		return r
							
	def display(self, output):
		print(output);
		
	def saveResult(self,tests,filename):
			#test output array
		out={}	
		out['hardware']={}
		out['hardware']['version']=self.version['hardware']
		out['hardware']['major']=self.version['hardwareMajor']
		out['firmware']={}
		out['firmware']['version']=self.version['firmware']
		out['firmware']['commit']=self.version['commit']
		out['tests']=tests #add based on device key....
		
		with open(filename, 'w') as outfile:
			json.dump(out, outfile, indent=4, sort_keys=False)
					
	def timeout(self, timeout=0.1):
		select.select({}, {}, {}, timeout)


t=testSuite(args['port'],115200,1)

version=t.getVersion()
result=[]

#are we loading a single script or a folder of scripts?
if os.path.isfile(args['test']):
	testStatus=t.importTest(args['test'])
	if testStatus == False:
		print('Skipped test, hardware version not supported!')
	else:
		result.append(t.run())
elif os.path.isdir(args['test']):
	for filename in glob.glob(os.path.join(args['test'], '*.json')):
		testStatus=t.importTest(filename)
		if testStatus == False:
			print('Skipped test, hardware version not supported!')
		else:
			result.append(t.run())

t.saveResult(result,args['result'])

if(args['upload']==True):
	uploadtempfilename='upload.json'
	temp = json.load(open(args['result']))
	temp['apikey']=args['apikey']
	with open(uploadtempfilename, 'w') as outfile:
		json.dump(temp, outfile, indent=4, sort_keys=False)
	with open(uploadtempfilename, 'rb') as f: 
		r = requests.post(args['url'], files={uploadtempfilename: f})	