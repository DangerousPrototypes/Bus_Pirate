#sample PERL script to dump SPI EEPROM with Bus Pirate firmware v2.5+ binary mode
# will work with 2.4, but there are some problems:
# 1. SPI CS bit is inversed 
# 2. CS command didn't return 1.
# This example is for educational use and it's totally free
#knowledge shouldn't be locked up, demo code shouldn't be licensed
#released into the Public Domain or CC0, your choice.

#NOTE:
# Connect a SPI EEPROM to the BUs Pirate as normal
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

#
#
#	GET COMMAND LINE OPTIONS
#
#default values
$eebytes=128;
$mysport="COM2";
$eebuflen="8";
$vregen="0";
$pullen="0";
$outname='dump.bin';
$addressbytes=2;

use Getopt::Std;
getopt ('rwsblvua');

if(($opt_r) && ($opt_r != 1)){
	$outname=$opt_r;
}elsif(($opt_w ne "") && ($opt_w != 1)){
	$outname=$opt_w;
}else{
print "# This is an example of possible features,\n"; 
print "#  We haven't implemented writes yet!\n";
print "# -r <file> read EEPROM to file\n";
print "# -w <file> write EEPROM from file (*not implemented)\n"; 
print "# -s serial port name or number (default COM2)\n";
print "# -b how many bytes to dump (default 128)\n";
print "# -a address length bytes (default 2)\n";
print "# -l write buffer length bytes (default 8)\n";
print "# -v enable power supplies during operation (*not implemented, always on)\n";
print "# -u enable pullup resistors, open drain operation (*not implemented)\n";
print "#\n# example: SPIeeprom.pl -s COM2 -r dump.bin -b10 -v\n# Reads 10 bytes to dump.bin from a Bus Pirate on COM2, power supply enabled.\n";
	die "No mode or file specified.";
}

if($opt_s ne ""){
	$mysport=$opt_s;
}
if($opt_b ne ""){
	$eebytes=$opt_b;
}
if($opt_l ne ""){
	$eebuflen=$opt_l;
}
if($opt_v ne ""){
	$vregen=$opt_v;
}
if($opt_u ne ""){
	$pullen=$opt_u;
}
if($opt_a ne ""){
	$addressbytes=$opt_a;
}
# Set up the serial port for Windows
use Win32::SerialPort;
my $port = Win32::SerialPort->new($mysport); #change to your com port
#setup serial port for Linux
#use Device::SerialPort;
#my $port = Device::SerialPort->new("$mysport"); #change to your com port

#port configuration  115200/8/N/1
$port->databits(8);
$port->baudrate(115200);
$port->parity("none");
$port->stopbits(1);
$port->buffers(1, 1); #1 byte or it buffers everything forever
$port->write_settings		|| undef $port; #set
unless ($port)			{ die "couldn't write_settings"; }

print "Bus Pirate SPI EEPROM PROGRAMMER v0.1\n";
print "Use Bus Pirate firmware v2.5+.\n";

#
#
#	Enter Bus Pirate binary mode, start raw SPI mode
#
#
#enter binmode
print "Entering binmode: ";
if(&enterBinMode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete operation.\n";
}

#now, enter raw SPI mode
print "Entering raw SPI mode: ";
if(&enterRawSPImode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete operation.\n";
}

#
#
#	Setup SPI
#
#
print "Configuring SPI.\n";
#peripherals
$port->write("\x49"); #0b0100 + power, CS high
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
	die "Failed to set SPI peripherals.";
}
#speed
$port->write("\x65"); #0b0110 + 250khz
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
	die "Failed to set SPI speed.";
}
#settings
$port->write("\x8A"); #0b1000 + normal output, clock edge=1
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
	die "Failed to set SPI configuration.";
}
select(undef,undef,undef, .2); #let power settle
#
#
# SPI EEPROM READ
#
#
print "Reading EEPROM.\n";

#enable CS (low)
#$port->write("\x02"); #send CS low command (bug in v2.4 doesn't return byte for this command, fixed in v2.5)
$port->write("\x48"); #0b0100 + power, CS low
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
	die "Failed to set CS.";
}

#setup the EEPROM for reads from the beginning address
#bulk read /write method
if($addressbytes==1){
	$port->write("\x11"); #bulk command for x bytes (10001 + address bytes)
}else{
	$port->write("\x12"); #bulk command for x bytes (10001 + address bytes)
}
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
	die "Failed bulk read/write.";
}
#0x03 is the read EEPROM command, 0x00 0x00 sets the address
$port->write("\x03");
$port->write("\x00");
if($addressbytes>1){
	$port->write("\x00"); # this address is optional on smaller EEPROMs
}
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
$port->read($addressbytes+1); #read and discard the three byte reply

#nibble method
#every time the low nibble is sent, the Bus Pirate reads/writes a byte
#we setup with 0xFF as a demo, but the value doesn't matter for reads
#setup high nibble , this is the upper 4bit of the byte
$port->write("\x3F"); #0x3f = high nibble command + 1111
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
	die "Failed to setup high nibble.";
}

open(OUTPUT, ">", $outname) or die $!;
#setup low nibble, read/write a byte on SPI. Repeat untill the end
while($eebytes){
	$port->write("\x2F");
	select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
	$pb=$port->read(1); #read and save the byte reply
	print OUTPUT $pb; 
	$eebytes--;
}

#CS disable (high again)
#$port->write("\x03"); #send CS high command (bug in v2.4 doesn't return byte for this command, fixed in v2.5)
$port->write("\x49"); #0b0100 + power, CS high
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
	die "Failed to set CS.";
}

#
#
# RESET Bus Pirate
#
#
print "Reset Bus Pirate to user terminal: ";
if(&exitBinMode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't exit binary mode.\n";
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