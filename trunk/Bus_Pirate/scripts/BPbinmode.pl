#sample PERL script to put Bus Pirate firmware v2.4+ into binary mode
# This example is for educational use and it's totally free
#knowledge shouldn't be locked up, demo code shouldn't be licensed
#released into the Public Domain or CC0, your choice.

#NOTE:
# This demo tests the interface between the PC and the Bus Pirate
#  It will enable the power supplies, pullups, and various pins, remove any attached devices
# Developed with ActivePerl: http://www.activestate.com/activeperl/
#
# For windows: configure your COM port below
# you'll also need Win32::Serialport http://search.cpan.org/~bbirth/Win32-SerialPort-0.19/lib/Win32/SerialPort.pm
#
# For others: use the Device::SerialPort version and modify the serial port name
# you'll need Device::SerialPort  http://search.cpan.org/~cook/Device-SerialPort-1.002/SerialPort.pm

#ANOTHER NOTE:
# This is my first time using Perl,
# I was lazy and used short delays to wait for incoming bytes.
# The proper way is probably with some kind of loop, buffer, system timeout, etc, etc, etc.

# Set up the serial port for Windows
use Win32::SerialPort;
my $port = Win32::SerialPort->new("COM2"); #change to your com port
#setup serial port for Linux
#use Device::SerialPort;
#my $port = Device::SerialPort->new("/"); #change to your com port

#port configuration  115200/8/N/1
$port->databits(8);
$port->baudrate(115200);
$port->parity("none");
$port->stopbits(1);
$port->buffers(1, 1); #1 byte or it buffers everything forever
$port->write_settings		|| undef $port; #set
unless ($port)			{ die "couldn't write_settings"; }

#bitbang command and pin definitions
#commands:
use constant SETIN => "\x40"; #set pin direction input(1) output (0), returns read
use constant SETON => "\x80"; #set pins on (1), returns read
# Bits are assigned as such:
use constant MOSI => "\x01";
use constant CLK => "\x02";
use constant MISO => "\x04";
use constant CS => "\x08";
use constant AUX => "\x10";
use constant PULLUP => "\x20";
use constant POWER => "\x40";

print "Bus Pirate binmode tests v1.\n";
print "Use Bus Pirate firmware v2.4+.\n";

print "Entering binmode: ";
if(&enterBinMode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}

print "Testing bitbang mode functions\n";

# BB pin direction
#set pin direction 
$bpcommand= (SETIN | AUX ); #AUX to input, all others to output
$port->write($bpcommand); #send command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
$bpreply= $port->read(1); #read one byte,

# BB peripherals, pins on/off
#this enables power supplies and pullups,
#be careful if you have something attached to the Bus Pirate!
$bpcommand = ( SETON | POWER | PULLUP | CS | MISO | CLK | MOSI); #set all but AUX to high state (3.3volts output)
$port->write($bpcommand); #send command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
$bpreply= $port->read(1); #read one byte,

# BB read results
if($bpreply & MOSI){ #we set these high output, so they should read 1
	print "MOSI high, OK\n";
}else{
	print "MOSI low, error!\n";
}

#aux is input and unattached, it could read high or low
if($bpreply & AUX){ 
	print "AUX high, doesn't matter.\n";
}else{
	print "AUX low, doesn't matter.\n";
}

# cleanup: pins, power supplies, turn everything off
$bpcommand = ( SETON); 
$port->write($bpcommand); #send command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
$bpreply= $port->read(1); #read one byte,


#test entering raw SPI mode from bitbang mode
print "Entering raw SPI mode: ";
if(&enterRawSPImode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}

# spi power, pullups
#see the SPIeerprom for SPI mode examples

#do bus pirate hardware reset
print "Reset Bus Pirate to user terminal: ";
if(&exitBinMode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}


###############################
#
#
# 		Helper functions
#
#
###############################
#this function ensures we're in binmode and enters SPI mode
#returns SPIx version number, or 0 for failure
sub enterRawSPImode {
	#this section is optional, it ensures that we're in binmode and that the Bus Pirate responds
	my $ver=&enterBinMode; #return to BBIO mode (0x00), (should get BBIOx)
	if($ver == 0){
		return 0; #failed, not in binmode
	}else{
		#print "(BBIO OK) "; #debug
	}
	
	#this part actually enters binmode
	$port->write("\x01"); #send 0x01 in binmode to enter raw SPI mode
	select(undef,undef,undef, .1); #sleep for fraction of second for data to arrive #sleep(1);
	$char= $port->read(4); #look for SPIx
	if($char){
		print "(" . $char . ") "; #debug
		if($char eq "SPI1"){#if we got text, is it BBIOx?
			return 1; #return version number
		}
	}
	return 0;

}

#Returns to user terminal mode from raw binary mode
#resets hardware  and exits binary mode
#returns BBIO version, or 0 for failure
sub exitBinMode{

	#make sure we're in BBIO (not spi, etc) binmode before sending reset command
	my $ver=&enterBinMode; #return to BBIO mode (0x00), (should get BBIOx)
	
	#if we're ready, send the reset command
	if($ver){
		$port->write("\x0F"); #send 0x0f to do a hardware reset
	}
	return $ver;
}

#this function puts the Bus Pirate in binmode
#returns binmode version number, 0 for failure
sub enterBinMode {
#it could take 1 or 20 0x00 to enter Bus Pirate binary mode
#it will take 20 if we're currently at the user terminal mode
#it will only take 1 if the Bus Pirate is already in a raw mode
#BP replies BBIOx where x is the protocol version
	
	my $count=40;
	my $char="";
	while($count){
		$port->write("\x00"); #send 0x00
		select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
		$char= $port->read(5); #look for BBIOx
		if($char){
			#print "(" . $char . ") "; #debug
			if($char eq "BBIO1"){#if we got text, is it BBIOx?
				return 1; #return version number
			}
		}
		$count--; #if timeout, then try again
	}
	return 0; #for fail, version number for success
}

#The Bus Pirate might be stuck in a configuration menu or something when we connect
#send <enter> 10 times, then #<enter> to reset the Bus Pirate
#need to pause and flush buffer when complete
sub userTerminalReset{
	$port->write("\n\n\n\n\n\n\n\n\n\n#\n");
	#now flush garbage from read buffer
}

#debug variable transformation
#$char =~ s/\cM/\r\n/; #debug