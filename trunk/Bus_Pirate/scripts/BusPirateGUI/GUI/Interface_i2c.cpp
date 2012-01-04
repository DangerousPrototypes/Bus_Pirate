#include <QtGui>
#include "BinMode.h"
#include "BPSettings.h"
#include "MainWin.h"
#include "Interface.h"
#include "Events.h"

I2CGui::I2CGui(MainWidgetFrame *parent) : QWidget(parent)
{
	this->parent = parent;

	QLabel *device_label = new QLabel("Device: ");
	QLabel *device_addr_read_label = new QLabel("Read Addr: ");
	QLabel *device_addr_write_label = new QLabel("Write Addr: ");
	QLabel *file_size_label = new QLabel("Data: ");
	QLabel *log_label = new QLabel("Log: ");
	QLabel *dev_prop_saddr_label = new QLabel("Start Mem Addr: ");

	QPushButton *scan = new QPushButton("Scan I2C");
	scan->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *read_btn = new QPushButton("Read I2C");
	read_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *write_btn = new QPushButton("Write I2C");
	write_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QVBoxLayout *vlayout = new QVBoxLayout;
	QHBoxLayout *hlayout = new QHBoxLayout;
	QHBoxLayout *dev_addr_layout = new QHBoxLayout;
	QHBoxLayout *dev_addr_layout2 = new QHBoxLayout;
	QHBoxLayout *dev_addr_layout3 = new QHBoxLayout;
	QHBoxLayout *file_line = new QHBoxLayout;
	
	QRegExp rx("^(0x){,1}[a-fA-f0-9]{2}$");
	QRegExp rx_int("^\\d{1,}[KMGkmg]{,1}");
	QRegExpValidator *hex_valid = new QRegExpValidator(rx, this);
	
	device_addr_write = new QLineEdit("0xA0");
	device_addr_write->setValidator(hex_valid);
	device_addr_write->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	device_addr_read = new QLineEdit("0xA1");
	device_addr_read->setValidator(hex_valid);
	device_addr_read->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//start_addr = new QLineEdit("0x00");
	//start_addr->setValidator(hex_valid);
	//start_addr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	file_size = new QLineEdit;
	file_size->setValidator(new QRegExpValidator(rx_int, this));

	msglog = new QTextEdit;
	msglog->setReadOnly(true);
	
	hlayout->addWidget(scan);
	hlayout->addWidget(read_btn);
	hlayout->addWidget(write_btn);

	dev_addr_layout->addWidget(device_addr_read_label);
	dev_addr_layout->addWidget(device_addr_read);
	dev_addr_layout->addWidget(device_addr_write_label);
	dev_addr_layout->addWidget(device_addr_write);
	
	//dev_addr_layout2->addWidget(dev_prop_saddr_label);
	//dev_addr_layout2->addWidget(start_addr);
	dev_addr_layout3->addWidget(file_size_label);
	dev_addr_layout3->addWidget(file_size);
	
	vlayout->addSpacing(10);
	vlayout->addWidget(device_label);
	vlayout->addLayout(dev_addr_layout);
	//vlayout->addLayout(dev_addr_layout2);
	vlayout->addLayout(dev_addr_layout3);
	vlayout->addSpacing(10);
	vlayout->addLayout(hlayout);
	vlayout->addSpacing(50);
	vlayout->addWidget(log_label);
	vlayout->addWidget(msglog);
	
	connect(scan, SIGNAL(clicked()), this, SLOT(search_i2c()));
	connect(read_btn, SIGNAL(clicked()), this, SLOT(read_i2c()));
	connect(write_btn, SIGNAL(clicked()), this, SLOT(write_i2c()));
	
	setLayout(vlayout);
}

void I2CGui::search_i2c()
{
	QString dev_addr;
	QString start_msg = "Getting I2C Devices...";
	QString end_msg = "Getting I2C Devices...Success!";
	int i = 0, dev=0, addr=0;
	char rw;
	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(start_msg));
	for (i=0; i<0x100; i++)
	{
		dev = i;
		addr = i>>1;

		parent->bp->i2c_start();
		parent->bp->serial->write((char*)&dev, 1);
		parent->bp->i2c_byte_read();
		if ((addr & 0x01)==0) 
		{
			rw = 'W';
		} else {
			parent->bp->i2c_byte_read();
			parent->bp->i2c_ack_send();
			rw = 'R';
		}
		parent->bp->i2c_stop();

		dev_addr = QString("%1 (%2 %3)").arg(dev, 0, 16).arg(addr, 0, 16).arg(rw);
		postMsgEvent(dev_addr.toAscii());
	}
	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(end_msg));
}

void I2CGui::write_i2c(void)
{
	bool ok;
	QByteArray data;
	QString start_msg = "Writing I2C Device...";
	QString end_msg = "Writing I2C Device...Done!";

	QFile qfile(file->text());
	if (!qfile.open(QIODevice::ReadOnly))
		return;

	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(start_msg));

	// i2c start
	parent->bp->i2c_start();
	// write chip address (w)
	parent->bp->serial->write((char*)device_addr_write->text().toInt(&ok, 16), 1);
	// write memory address
	parent->bp->serial->write((char*)start_addr->text().toInt(&ok, 16), 1);
	// values to write
	data = qfile.readAll();
	parent->bp->serial->write(data.data(), sizeof(data.data()));
	// i2c stop
	parent->bp->i2c_stop();

	qfile.close();
	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(end_msg));
}

void I2CGui::read_i2c(void)
{
	int i=0;
	int fsize = file_size->text().toInt();
	bool more = false;
	bool ok;
	QByteArray data;
	QString start_msg = "Reading I2C Device...";
	QString end_msg = "Reading I2C Device...Done!";

	QFile qfile(file->text());
	if (!qfile.open(QIODevice::WriteOnly))
		return;

	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(start_msg));
	// i2c start
	parent->bp->i2c_start();
	// write chip (w) address
	parent->bp->serial->write((char*)device_addr_write->text().toInt(&ok, 16), 1);
	// write memory address
	parent->bp->serial->write((char*)start_addr->text().toInt(&ok, 16), 1);
	// i2c stop
	parent->bp->i2c_stop();

	// i2c start
	parent->bp->i2c_start();
	// write chip (r) address
	parent->bp->serial->write((char*)device_addr_read->text().toInt(&ok, 16), 1);

	for (i=0; i<fsize; i++)
	{
		// read byte
		data = parent->bp->i2c_byte_read();
		qfile.write(data);
		// send ack (if for more) else send nack
		if (i<fsize) parent->bp->i2c_ack_send();
		
	}
	parent->bp->i2c_nack_send();
	// i2c stop
	parent->bp->i2c_stop();
	qfile.close();
	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(end_msg));
}

void I2CGui::customEvent(QEvent *ev)
{
	if (static_cast<BPEventType>(ev->type()) == I2CLogMsgEventType)
	{
		msglog->append(dynamic_cast<I2CLogMsgEvent* >(ev)->msg);
	}
}

void I2CGui::postMsgEvent(const char* msg)
{
	QString qmsg = QString(msg);
	QCoreApplication::sendEvent(this, new I2CLogMsgEvent(qmsg));
}

