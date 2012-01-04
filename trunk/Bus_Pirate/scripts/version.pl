#sample PERL script to get and parse Bus Pirate version string from firmware v2.8+
# version.pl -h1 for help
# This example is for educational use and it's totally free
#knowledge shouldn't be locked up, demo code shouldn't be licensed
#released into the Public Domain or CC0, your choice.

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

use Getopt::Std;
getopt ('ph');

print "Getting version string.\n";

$mysport="COM2";

if($opt_h ne ""){
	print "# Get version string. Use with firmware v2.6+. \n";
	print "# -h1 This screen.\n";
	print "# -p Serial port name or number (default COM2).\n";
	print "# Example: version.pl -p COM2\n"; 
	die "";
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

#exit bin mode
print "Exiting binmode: ";
if(&exitBinMode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}

select(undef,undef,undef, .2); #sleep for fraction of second for data to 
print "Version info:\n";
$version=$port->read(1000);

my @values = split('\x0d\x0a', $version);
#$value[1] is a line feed
#hardware version
print "Hardware: " . $values[1] . "\n";
#hardware version
print "Firmware: " . $values[2] . "\n";
#hardware version
print "PIC chip: " . $values[3] . "\n";
#news and updates
print "Updates URL: " . $values[4] . "\n";
#print $version;

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
		select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
		$char= $port->read(1); #look for BBIOx
		if($char && ($char eq "\x01") ){
			return 1; #return version number
		}

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
		if($char && ($char eq "BBIO1") ){
			return 1; #return version number
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
	
	if($char && ($char eq "BBIO1")){
		#print "(" . $char . ") "; #debug
		return 1; #return version number
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