#ifndef __IFACE_H
#define __IFACE_H

#include <QtGui>

enum bbio_pins
{
	BB_POWER = 0x08,
	BB_PULLUPS = 0x04,
	BB_AUX = 0x02,
	BB_CS = 0x01
};

enum bbio_io_pins
{
	BBIO_MOSI = 0x01,
	BBIO_CLK = 0x02,
	BBIO_MISO = 0x04,
	BBIO_CS = 0x08,
	BBIO_AUX = 0x10,
	BBIO_PULLUP = 0x20,
	BBIO_POWER = 0x40
};

enum spi_cfg
{
	SPI_OUT_TYPE = 0x08,
	SPI_IDLE = 0x04,
	SPI_CLK_EDGE = 0x02,
	SPI_SAMPLE = 0x01
};

class MainWidgetFrame;
/*class SpiGui : public QWidget
{
Q_OBJECT
public:
	SpiGui(MainWidgetFrame *p);
private slots:
	void read_spi();
	void write_spi();
	void spi_chip_id();
private:
	MainWidgetFrame *parent;
	QLineEdit *file;
	QTextEdit *msglog;
protected:
	virtual void customEvent(QEvent *ev);
public:
	void postMsgEvent(const char* msg);
};*/

class I2CGui : public QWidget
{
Q_OBJECT
public:
	I2CGui(MainWidgetFrame *p);
private:
	MainWidgetFrame *parent;
	QLineEdit *device_addr_read;
	QLineEdit *device_addr_write;
	QLineEdit *file;
	QLineEdit *file_size;
	QLineEdit *start_addr;
	QTextEdit *msglog;
private slots:
	void search_i2c(void);
	void write_i2c(void);
	void read_i2c(void);
protected:
	virtual void customEvent(QEvent *ev);
public:
	void postMsgEvent(const char* msg);
};

class OneWireGui : public QWidget
{
public:
	OneWireGui(MainWidgetFrame *p);
private:
	MainWidgetFrame *parent;
	QLineEdit *device_addr;
	QLineEdit *file;
	QTextEdit *msglog;
	QLineEdit *dev_rom_read;
	QLineEdit *dev_rom_skip;
	QLineEdit *dev_rom_match;
	QLineEdit *dev_rom_search;
	QLineEdit *dev_scratch_read;
	QLineEdit *dev_scratch_write;
	QLineEdit *dev_scratch_copy;
	QLineEdit *dev_mem_read;
	QLineEdit *dev_mem_write;
	QLineEdit *dev_status_read;
	QLineEdit *dev_status_write;
	
protected:
	virtual void customEvent(QEvent *ev);
public:
	void postMsgEvent(const char* msg);
};

class RawWireGui : public QWidget
{
public:
	RawWireGui(MainWidgetFrame *p);
private:
	MainWidgetFrame *parent;
	QLineEdit *device_addr;
	QLineEdit *file;
	QTextEdit *msglog;
protected:
	virtual void customEvent(QEvent *ev);
public:
	void postMsgEvent(const char* msg);
};

class JtagGui : public QWidget
{
Q_OBJECT
public:
	JtagGui(MainWidgetFrame *p);
private:
	MainWidgetFrame *parent;
	QLineEdit *device_addr;
	QLineEdit *file;
	QTextEdit *msglog;
protected:
	virtual void customEvent(QEvent *ev);
public:
	void postMsgEvent(const char* msg);
};

class RawTextGui : public QWidget
{
Q_OBJECT
protected:
	virtual void customEvent(QEvent *ev);
public:
	RawTextGui(MainWidgetFrame *p);
	void postMsgEvent(const char* msg);
private slots:
	void ExecuteFile(void);
private:
	QTextEdit *msglog;
	QLineEdit *raw_file;
	MainWidgetFrame *parent;
};

/*class PowerGui : public QWidget
{
Q_OBJECT
public:
	PowerGui(MainWidgetFrame *p);
private slots:
	void hiz_power_enable();
	void open_collector_power();
	void normal_power_enable();
	void hardReset();
	void userTermReset();
	void enterMode();
	void resetBBIO();
	void setupBusPirate();
	void getBuffer();
private:
	MainWidgetFrame *parent;
};*/

#endif

/* 1-Wire:
#define W1_ROM_SEARCH    0xF0
#define W1_ROM_MATCH     0x55
#define W1_ROM_READ      0x33
#define W1_ROM_SKIP      0xCC
#define W1_OVRDRV_SKIP_ROM   0x3C
#define W1_OVRDRV_MATCH_ROM  0x69
#define W1_SCRATCH_READ  0xAA
#define W1_SCRATCH_WRITE 0x00
#define W1_SCRATCH_COPY  0x55
#define W1_STATUS_READ   0x00
#define W1_STATUS_WRITE  0x00
#define W1_PASSWD_WRITE  0x00
#define W1_MEM_READ      0xF0
#define W1_MEM_WRITE     0x00
#define W1_EXT_MEM_READ  0xA5
#define W1_SUBKEY_READ   0x66

#define READ_ROM 0x33
#define READ_ROM_ALT 0x0F
#define SKIP_ROM 0xCC
#define MATCH_ROM 0x55
#define SEARCH_ROM 0xF0
#define OVRDRV_SKIP_ROM 0x3C
#define OVRDRV_MATCH_ROM 0x69
#define READ_MEM 0xF0
#define EXT_READ_MEM 0xA5
#define READ_SUBKEY 0x66
#define WRITE_SCRATCHPAD 0x0F
#define WRITE_SCRATCHPAD_ALT 0x96
#define READ_SCRATCHPAD 0xAA
#define READ_SCRATCHPAD_ALT 0x69
#define COPY_SCRATCHPAD 0x55
#define COPY_SCRATCHPAD_ALT 0x3C
#define WRITE_SUBKEY 0x99
#define WRITE_PASSWD 0x5A
#define WRITE_MEM 0x0F
#define WRITE_STATUS 0x55
#define READ_STATUS 0xAA
*/
