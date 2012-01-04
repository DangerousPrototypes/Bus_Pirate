#ifndef __BINMODE_H
#define __BINMODE_H

#include "qextserialport/qextserialport.h"

#define     WREN         0x06 // A:0 U:0 D:0
#define     WRDI         0x04 // A:0 U:0 D:0
#define     RDID         0x9F // A:0 U:0 D:3
#define     RDSR         0x05 // A:0 U:0 D:inf
#define     READ         0x03 // A:3 U:0 D:inf
#define     FAST_READ    0x0B // A:3 U:1 D:inf
#define     PW           0x0A // A:3 U:0 D:256
#define     PP           0x02 // A:3 U:0 D:256
#define     PE           0xDB // A:3 U:0 D:0
#define     SE           0xD8 // A:3 U:0 D:0
#define     DP           0xB9 // A:0 U:0 D:0
#define     RDP          0xAB // A:0 U:0 D:0

class MainWidgetFrame;
class BinMode : public QWidget
{
public:
	/* Construct */
	BinMode(MainWidgetFrame *ss);
	~BinMode();
	
	/* Command Method */
	QByteArray command(unsigned short command);
	
	/* Dump Buffers */
	QByteArray dumpBuffer();

	/* BBIO */
	int        enter_mode_bbio(void);
	int        reset_bbio(void);
	QByteArray reset_hardware(void);
	QByteArray reset_user_terminal(void);

	/* Interface Entry Methods */ 
	int        enter_mode_spi(void);
	int        enter_mode_i2c(void);
	int        enter_mode_uart(void);
	int        enter_mode_onewire(void);

	/* BBIO pin settings */
	int        raw_set_io(unsigned short pins);
	int        raw_set_pins(unsigned short pins);

	/* Self Test Methods */
	QByteArray test_mode_short(void);
	QByteArray test_mode_long(void);

	/* Common Interface Methods */
	QByteArray bbio_mode_version(void);        
	QByteArray bbio_bulk_trans(QByteArray data, unsigned short size);
	int        bbio_speed_set(unsigned short speed);
	QByteArray bbio_speed_read(void);
	int        bbio_peripherial_set(unsigned short pins);
	QByteArray bbio_peripherial_read(void);

	/* SPI */
	int        spi_cs_low(void);
	int        spi_cs_high(void);
	int        spi_nibble_high(unsigned short nibble);
	QByteArray spi_nibble_low(unsigned short nibble);
	int        spi_configure_set(unsigned short spi_cfg);
	QByteArray spi_configure_read(void);

	/* I2C */
	int        i2c_start(void);
	int        i2c_stop(void);
	QByteArray i2c_byte_read(void);
	int        i2c_ack_send(void);
	int        i2c_nack_send(void);

	/* Serial Port Access */
	QextSerialPort *serial;
	MainWidgetFrame *parent;
public slots:
	/* Port Manipulation */
	bool       port_open(void);
	void       port_close(void);
};

#endif

