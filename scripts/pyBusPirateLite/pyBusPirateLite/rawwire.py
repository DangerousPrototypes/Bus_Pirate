from .BinBang import *

class RawWireCfg:
	NA = 0x01
	LSB = 0x02
	_3WIRE = 0x04
	OUTPUT = 0x08
	
class RawWire(BBIO):
	def __init__(self, port, speed):
		BBIO.__init__(self, port, speed)
		
	def start_bit(self):
		self.port.write("\x02")
		self.timeout(0.1)
		return self.response(1)
		
	def stop_bit(self):
		self.port.write("\x03")
		self.timeout(0.1)
		return self.response(1)
		
	def cs_low(self):
		self.port.write("\x04")
		self.timeout(0.1)
		return self.response(1)
		
	def cs_high(self):
		self.port.write("\x05")
		self.timeout(0.1)
		return self.response(1)
	
	def read_byte(self):
		self.port.write("\x06")
		self.timeout(0.1)
		return self.response(1)

	def read_bit(self):
		self.port.write("\x07")
		self.timeout(0.1)
		return self.response(1)
			
	def peek(self):
		self.port.write("\x08")
		self.timeout(0.1)
		return self.response(1)
	
	def clock_tick(self):
		self.port.write("\x09")
		self.timeout(0.1)
		return self.response(1)
		
	def clock_low(self):
		self.port.write("\x0A")
		self.timeout(0.1)
		return self.response(1)
		
	def clock_high(self):
		self.port.write("\x0B")
		self.timeout(0.1)
		return self.response(1)
		
	def data_low(self):
		self.port.write("\x0C")
		self.timeout(0.1)
		return self.response(1)
		
	def data_high(self):
		self.port.write("\x0D")
		self.timeout(0.1)
		return self.response(1)
		
	def wire_cfg(self, pins=0):
		self.port.write(0x80 | pins)
		self.timeout(0.1)
		return self.response(1)
		
	def bulk_clock_ticks(self, ticks=1):
		self.port.write(0x20 | (ticks-1))
		self.timeout(0.1)
		return self.response(1)
		