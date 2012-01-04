#sample PERL script to put Bus Pirate firmware v2.6+ into binary mode and run self-test
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
#commands:
use constant SELFTEST_SHORT => "\x10";
use constant SELFTEST_FULL => "\x11";
use constant SELFTEST_EXIT => "\xFF";

use Getopt::Std;
getopt ('pt');

print "Starting self test.\n";
print "Disconnect any devices.\n";

$mysport="COM2";

if($opt_t ne ""){
	if($opt_t eq "f"){
		print "Full test: connect (Vpu to +5V) and (ADC to +3.3V).\n";
		$bpcommand= (SELFTEST_FULL);
	}else{
		print "Short test: pull-up, ADC, Vpu not tested!\n";
		$bpcommand= (SELFTEST_SHORT);
	}
	print "\n";
}else{
	print "# Bus Pirate v2go, v3 self-test.\n"; 
	print "# Use with firmware v2.6+\n";
	print "# Disconnect any devices.\n";
	print "# -p Serial port name or number (default COM2)\n";
	print "# -tf Full self-test, connect (Vpu to +5V) and (ADC to +3.3V).\n";
	print "# -ts Short self-test, no jumpers required.\n"; 
	print "# Example: selftest.pl -p COM2 -ts\n"; 
	die "No mode specified, use -tf or -ts.";
}

if($opt_p ne ""){
	$mysport=$opt_p;
}

# Set up the serial port for Windows
use Win32::SerialPort;
my $port = Win32::SerialPort->new($mysport); #change to your com port
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

print "Entering binmode: ";
if(&enterBinModefast==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}

print "Running self-test: ";
$port->write($bpcommand); #send command
#self test takes some time, wait for byte
$count=20;
while($count){
	select(undef,undef,undef, .2); #sleep for fraction of second for data to arrive #sleep(1);
	$bpreply= $port->read(1); #read one byte,
	if($bpreply ne ""){ #got a byte from Bus Pirate, test over
		$count=0;#break the loop
	}else{
		$count--;
		if($count==0){
			die "Self-test timeout!\n";
		}
	}	
}

if($bpreply eq "\x00"){
	print "OK :)\n";
}else{
	print "WARNING: ERRORS! FAILED!\n";
}

#exit self test mode
print "Exiting self-test mode:";
$port->write(SELFTEST_EXIT); #send command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) == "\x01"){#read one byte,
	print "OK\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}

#exit bin mode
print "Exiting binmode: ";
if(&exitBinMode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}

if($bpreply eq "\x00"){
	print "\nSelf-test OK :)\n";
}else{
	print "\nWARNING: ERRORS! SELF-TEST FAILED!\n";
}

###############################
#
#
# 		Helper functions
#
#
###############################

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

sub enterBinModefast {
#it could take 1 or 20 0x00 to enter Bus Pirate binary mode
#this just blasts 20 0x00, then reads five and discards the rest. 
#BP replies BBIOx where x is the protocol version
	
	my $count=20;
	my $char="";
	
	while($count){
		$port->write("\x00"); #send 0x00
		$count--;
	}
	
	select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
	
	$char= $port->read(5); #look for BBIOx
	
	if($char){
		#print "(" . $char . ") "; #debug
		if($char eq "BBIO1"){#if we got text, is it BBIOx?
			return 1; #return version number
		}
	}

	$char= $port->read(); #flush buffer, could have 20 x 0x00
		
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