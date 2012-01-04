Latest instructions and version:
http://dangerousprototypes.com/docs/Bus_Pirate_LCD_adapter_v2#Production_testing_procedure

Setup a Bus Pirate for testing
#Use Bus Pirate with firmware v5.x
#Connect the Bus Pirate to USB
#Run lcdtest.exe -d COMx where COMx is the Bus Pirate serial port.

Test procedure
#Connect the LCD adapter the the Bus Pirate
#Press space to run the test
#Verify LED1 on the adapter is blinking (other outputs will also blink)
#Press space to reset the test
#Remove the LCD adapter
#Repeat with #1

Possible no LED solutions
*Check LED1, R2 (solder, failed)
*Check IC1 (solder, failed)
*Check IO connector (solder)