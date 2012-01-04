#include <QtGui>
#include "qextserialport/qextserialport.h"
#include "Events.h"
#include "MainWin.h"
#include "BPSettings.h"
#include "BinMode.h"

BinMode::BinMode(MainWidgetFrame *parent) : QWidget(parent)
{
	this->parent = parent;
	serial = new QextSerialPort("/dev/bus_pirate", QextSerialPort::Polling);
}

BinMode::~BinMode()
{
	port_close();
}

QByteArray BinMode::dumpBuffer()
{
	QByteArray resp;
	qDebug() << "Dump Buffers";
	do {
		serial->flush();
		resp = serial->readAll();
		serial->flush();
		qDebug() << resp.data();
	} while (!resp.isEmpty());
	return resp;
}

/* Port Manipulation */
bool BinMode::port_open()
{
	qDebug() << "port_open" << parent->settings->s_port->text();
	serial->setPortName(parent->settings->s_port->text());
	serial->setBaudRate((BaudRateType)parent->settings->usable_baud_rate->value(parent->settings->s_baud->currentText(), BAUD115200)); //BAUD115200
	serial->setDataBits((DataBitsType)parent->settings->s_databits->currentIndex());  //DATA_8
	serial->setStopBits((StopBitsType)parent->settings->s_stopbits->currentIndex());  //STOP_1
	serial->setParity((ParityType)parent->settings->s_parity->currentIndex());      //PAR_NONE
	serial->setFlowControl((FlowType)parent->settings->s_flow->currentIndex());      //FLOW_OFF
	serial->setTimeout(100);
	bool ret = serial->open(QIODevice::ReadWrite);
	serial->flush();
	qDebug() << "Serial Port Closed:" << serial->portName() << "is open-" << serial->isOpen();
	return ret;
}

void BinMode::port_close()
{
	serial->flush();
	serial->close();
	qDebug() << "Serial Port Closed:" << serial->portName() << "is open-" << serial->isOpen();
}

QByteArray BinMode::command(unsigned short command)
{
	char data = (command);
	QByteArray res;
	serial->flush();
	serial->write(&data, 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	return res;
}

/* BBIO */
int BinMode::enter_mode_bbio(void)
{
	int ret=0;
	QByteArray res;
	if (reset_bbio()) return ret;
	serial->flush();
	serial->write("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 20);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("BBIO")) ret = 1;
	if (ret) qDebug() << "BBIO Ready!";
	return ret;
}

int BinMode::reset_bbio(void)
{
	QByteArray version_string;
	int ret = 0;

	serial->flush();
	serial->write("\x00", 1);
	serial->flush();
	version_string = serial->readAll();
	serial->flush();
	if (version_string.contains("BBIO")) ret = 1;
	qDebug() << "BBIO - text:" << version_string;
	return ret;
}

QByteArray BinMode::reset_hardware(void)
{
	QByteArray buspirate_info, resp;
	serial->flush();
	serial->write("\x0F", 1);
	serial->flush();
	serial->read(1);
	serial->flush();
	buspirate_info = serial->readAll();
	serial->flush();
	qDebug() << "reset BP:" << buspirate_info;
	return buspirate_info;
}

QByteArray BinMode::reset_user_terminal(void)
{
	QByteArray resp;
	serial->flush();
	serial->write("\n\n\n\n\n\n\n\n\n\n#\n", 12);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	qDebug() << "reset user term:" << resp;
	return resp;
}

/* Interface Entry Methods */
int BinMode::enter_mode_spi(void)
{
	QByteArray version_string;
	int ret = 0;
	serial->flush();
	serial->write("\x01", 1);
	serial->flush();
	version_string = serial->readAll();
	serial->flush();
	if (version_string.contains("SPI")) ret = 1;
	qDebug() << "SPI - text: " << version_string;
	return ret;
}

int BinMode::enter_mode_i2c(void)
{
	int ret = 0;
	QByteArray version_string;
	serial->flush();
	serial->write("\x02", 1);
	serial->flush();
	version_string = serial->readAll();
	serial->flush();
	if (version_string.contains("I2C")) ret = 1;
	qDebug() << "I2C text: " << version_string;
	return ret;
}

int BinMode::enter_mode_uart(void)
{
	int ret = 0;
	QByteArray version_string;
	serial->flush();
	serial->write("\x03", 1);
	serial->flush();
	version_string = serial->readAll();
	serial->flush();
	if (version_string.contains("ART")) ret = 1;
	qDebug() << "UART text: " << version_string;
	return ret;
}

int BinMode::enter_mode_onewire(void)
{
	int ret = 0;
	QByteArray version_string;
	serial->flush();
	serial->write("\x04", 1);
	serial->flush();
	version_string = serial->readAll();
	serial->flush();
	if (version_string.contains("1W")) ret = 1;
	qDebug() << "1Wire text: " << version_string;
	return ret;
}

/* BBIO Pin Settings */
int BinMode::raw_set_io(unsigned short pins)
{
	int ret = 0;
	char data = (0x40|pins);
	QByteArray resp;
	serial->flush();
	serial->write(&data, 1);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	if (resp.contains("\x01")) ret = 1;
	qDebug() << "raw set io:" << data;
	return ret;
}

int BinMode::raw_set_pins(unsigned short pins)
{
	int ret = 0;
	char data = (0x80|pins);
	QByteArray resp;
	serial->flush();
	serial->write(&data, 1);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	if (resp.contains("\x01")) ret = 1;
	qDebug() << "raw set pins:" << data;
	return ret;
}

/* Self Test Methods */
QByteArray BinMode::test_mode_short(void)
{
	QByteArray resp;
	serial->flush();
	serial->write("\x10", 1);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	return resp;
}

QByteArray BinMode::test_mode_long(void)
{
	QByteArray resp;
	serial->flush();
	serial->write("\x11", 1);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	return resp;
}

/* Common Interfaces Methods */
QByteArray BinMode::bbio_mode_version(void)
{
	QByteArray resp;
	serial->flush();
	serial->write("\x01", 1);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	return resp;
}

QByteArray BinMode::bbio_bulk_trans(QByteArray data, unsigned short size)
{
	int i = 0;
	QByteArray cmd_response;
	QByteArray data_response;
	char cmd_data = (0x10|(size-1));
	serial->flush();
	serial->write(&cmd_data, 1);
	serial->flush();
	cmd_response = serial->readAll();
	serial->flush();
	serial->write(data, size);
	serial->flush();
	data_response = serial->readAll();
	serial->flush();
	if (cmd_response.contains("\x01")) return data_response;
	qDebug() << "bulk trans:" << data_response.toHex();
	return data_response;
}

int BinMode::bbio_speed_set(unsigned short speed)
{
	char data = (0x60|speed);
	QByteArray res;
	int ret = 0;
	serial->flush();
	serial->write(&data, 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret=1;
	return ret;
}

QByteArray BinMode::bbio_speed_read(void)
{
	QByteArray res;
	serial->flush();
	serial->write("\x70");
	serial->flush();
	res = serial->readAll();
	serial->flush();
	return res;
}

int BinMode::bbio_peripherial_set(unsigned short pins)
{
	char data = (0x40|pins);
	int ret = 0;
	bool Ok;
	QByteArray res;
	serial->flush();
	serial->write(&data, 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret=1;
	return ret;
}

QByteArray BinMode::bbio_peripherial_read(void)
{
	QByteArray res;
	serial->flush();
	serial->write("\x50");
	serial->flush();
	res = serial->readAll();
	serial->flush();
	return res;
}

/* SPI methods */
int BinMode::spi_cs_low(void)
{
	int ret = 0;
	QByteArray res;
	serial->flush();
	serial->write("\x02", 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret = 1;
	return ret;
}

int BinMode::spi_cs_high(void)
{
	int ret = 0;
	QByteArray res;
	serial->flush();
	serial->write("\x03", 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret=1;
	return ret;
}

int BinMode::spi_nibble_high(unsigned short nibble)
{
	int ret = 0;
	QByteArray resp;
	char nib = (0x30|nibble);
	serial->flush();
	serial->write(&nib, 1);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	if (resp.contains("\x01")) ret = 1;
	return ret;
}

QByteArray BinMode::spi_nibble_low(unsigned short nibble)
{
	QByteArray ret;
	char nib = (0x20|nibble);
	serial->flush();
	serial->write(&nib, 1);
	serial->flush();
	ret = serial->readAll();
	serial->flush();
	return ret;
}

int BinMode::spi_configure_set(unsigned short spi_cfg)
{
	char data = (0x80|spi_cfg);
	int ret = 0;
	QByteArray res;
	serial->flush();
	serial->write(&data, 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret=1;
	return ret;
}

QByteArray BinMode::spi_configure_read(void)
{
	QByteArray resp;
	serial->flush();
	serial->write("\x90", 1);
	serial->flush();
	resp = serial->readAll();
	serial->flush();
	return resp;
}

/* I2C Methods */
int BinMode::i2c_start(void)
{
	int ret = 0;
	QByteArray res;
	serial->flush();
	serial->write("\x02", 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret = 1;
	return ret;
}

int BinMode::i2c_stop(void)
{
	int ret = 0;
	QByteArray res;
	serial->flush();
	serial->write("\x03", 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret = 1;
	return ret;
}

QByteArray BinMode::i2c_byte_read(void)
{
	QByteArray res;
	serial->flush();
	serial->write("\x04", 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	return res;
}

int BinMode::i2c_ack_send(void)
{
	int ret = 0;
	QByteArray res;
	serial->flush();
	serial->write("\x05", 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret = 1;
	return ret;
}

int BinMode::i2c_nack_send(void)
{
	int ret = 0;
	QByteArray res;
	serial->flush();
	serial->write("\x06", 1);
	serial->flush();
	res = serial->readAll();
	serial->flush();
	if (res.contains("\x01")) ret = 1;
	return ret;
}

