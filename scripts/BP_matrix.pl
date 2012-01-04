#sample PERL script to demo 0832 Matrix Display
#http://whereisian.com/forum/index.php?topic=290.msg2326#msg2326
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

# Set up the serial port for Windows
#use Win32::SerialPort;
#my $port = Win32::SerialPort->new("COM2"); #change to your com port
#setup serial port for Linux
use Device::SerialPort;
my $port = Device::SerialPort->new("/dev/cu.usbserial-A7004E19"); #change to your com port

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

print "Use Bus Pirate firmware v2.4+.\n";

print "Entering binmode: ";
if(&enterBinMode==1){
	print "OK.\n";
}else{
	print "failed.\n";
	die "Couldn't complete test.\n";
}

print "Entering raw wire mode: ";
if(&enterRawWiremode==1){
        print "OK.\n";
}else{
        print "failed.\n";
        die "Couldn't complete test.\n";
}

print "Configuring Mode: \n";
#config settings - 0x84 - HiZ,3-wire,MSB
$bpcommand= ("\x84"); #
$port->write($bpcommand); #send command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
$bpreply= $port->read(1); #read 1 bytes,
if($port->read(1) eq "\x00"){#get byte, should be 1 for 0k, 0 for failure
        die "Failed to configure mode.";
}


print "Configuring Peripherals: \n";
#configure peripherals - 0x4C - Power On, PullUp on
$bpcommand= ("\x4C"); #
$port->write($bpcommand); #send command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);

if(!($port->read(1) eq "\x01")){#get byte, should be 1 for Ok, 0 for failure
        die "Failed to configure peripherals.";
}


&sendHT1632Cmd("\x18");#24 
&sendHT1632Cmd("\x28");#40 for a 8x32 matrix board. send 44 for a 16x24 matrix board
&sendHT1632Cmd("\x08");#8 BLINK_OFF
&sendHT1632Cmd("\x01");#1 SYS_EN
&sendHT1632Cmd("\x03");#3 LED_ON
&sendHT1632Cmd("\xAE");#174 Brightness PWM
&sendHT1632Cmd("\xA0");#160 Set brightness to low

&sendHT1632DataByte(0x00,"\x81");
&sendHT1632DataByte(0x10,"\x81");
&sendHT1632DataByte(0x20,"\x81");
&sendHT1632DataByte(0x30,"\x81");
sleep(1);

my @charB = ("\x00","\xFE","\x92","\x92","\x92","\x6C","\x00","\x00");

&sendHT1632DataChar(0x00,@charB);
&sendHT1632DataChar(0x10,"\x00","\xFE","\x90","\x90","\x90","\x60","\x00","\x00");#P
&sendHT1632DataChar(0x20,"\x00","\x38","\x04","\x02","\x04","\x38","\x00","\x00");#v
&sendHT1632DataChar(0x30,"\x00","\x84","\x82","\xA2","\xD2","\x8C","\x00","\x00");#3



print "Waiting for 10 seconds before powering off!\n";
sleep(10);

#Do bus pirate hardware reset
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
#Send [-^_^^ $cmd ^ ]
sub sendHT1632Cmd {
    my $cmd=shift;
    printf("Sending Command: 0x%02X ...",ord($cmd));
    $port->write("\x04"); #CS Low
    $port->write("\x0d"); #Data High
    $port->write("\x09"); #Clock Tick
    $port->write("\x0c"); #Data Low
    $port->write("\x09"); #Clock Tick
    $port->write("\x09"); #Clock Tick
    $port->write("\x10"); #Send 1 Byte
    $port->write($cmd); #The Command Byte
    $port->write("\x09"); #Clock Tick
    $port->write("\x05"); #CS High
    print "done\n";

}

#Send [-^_^-^ $addr $data]
#Only 7bits of addr is used
sub sendHT1632DataByte {
    my ($addr,$data)=@_;
    printf("Sending Data: 0x%02X to Addr: 0x%02X ...",$data,$addr);
    $port->write("\x04"); #CS Low
    $port->write("\x0d"); #Data High
    $port->write("\x09"); #Clock Tick
    $port->write("\x0c"); #Data Low
    $port->write("\x09"); #Clock Tick
    $port->write("\x0d"); #Data High
    $port->write("\x09"); #Clock Tick
    #Shift out the 7-bits from $addr
    for ($count=1; $count<8; $count++)
    {
	if (($addr & 64)>>6)
	{
	    $port->write("\x0d"); #Data high
	    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
	}else{
	    $port->write("\x0c"); #Data low
	    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
	}
 	$addr = $addr << 1;	
	$port->write("\x09"); #Clock Tick	
	select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
    }
    
    $port->write("\x10"); #Send 1 byte
    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
    $port->write($data);  #Data
    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
    $port->write("\x05"); #CS High
    print "done\n";
}

#Send 8 bytes to the matrix - ideal for characters!
sub sendHT1632DataChar {
    my ($addr,@char)=@_;
    printf("Sending Char to Addr 0x%02X ...",$addr);
    $port->write("\x04"); #CS Low
    $port->write("\x0d"); #Data High
    $port->write("\x09"); #Clock Tick
    $port->write("\x0c"); #Data Low
    $port->write("\x09"); #Clock Tick
    $port->write("\x0d"); #Data High
    $port->write("\x09"); #Clock Tick
    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
    #Shift out the 7-bits from $addr
    for ($count=1; $count<8; $count++)
    {
	if (($addr & 64)>>6)
	{
	    $port->write("\x0d"); #Data high
	    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
	}else{
	    $port->write("\x0c"); #Data low
	    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
	}
 	$addr = $addr << 1;	
	$port->write("\x09"); #Clock Tick	
	select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive
    }
    
    $port->write("\x17"); #Send 8 bytes
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[0]);  #Column 1
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[1]);  #Column 2
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[2]);  #Column 3
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[3]);  #Column 4
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[4]);  #Column 5
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[5]);  #Column 6
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[6]);  #Column 7
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write($char[7]);  #Column 8
    select(undef,undef,undef, .01); #sleep for fraction of second for data to arrive
    $port->write("\x05"); #CS High
    select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive

    print "done\n";
}

#this function ensures we're in binmode and enters raw wire mode
#returns SPIx version number, or 0 for failure
sub enterRawWiremode {
	#this section is optional, it ensures that we're in binmode and that the Bus Pirate responds
	my $ver=&enterBinMode; #return to BBIO mode (0x00), (should get BBIOx)
	if($ver == 0){
		return 0; #failed, not in binmode
	}else{
		#print "(BBIO OK) "; #debug
	}
	
	#this part actually enters binmode
	$port->write("\x05"); #send 0x01 in binmode to enter raw wire mode
	select(undef,undef,undef, .1); #sleep for fraction of second for data to arrive #sleep(1);
	$char= $port->read(4); #look for RAW1
	if($char){
		print "(" . $char . ") "; #debug
		if($char eq "RAW1"){#if we got text, is it BBIOx?
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
