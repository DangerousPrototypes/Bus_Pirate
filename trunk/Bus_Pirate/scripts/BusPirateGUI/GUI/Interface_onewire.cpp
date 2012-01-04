#include <QtGui>
#include "BinMode.h"
#include "BPSettings.h"
#include "MainWin.h"
#include "Interface.h"
#include "Events.h"

OneWireGui::OneWireGui(MainWidgetFrame *p) : QWidget(p)
{
	QLabel *device_label = new QLabel("Device: ");
	QLabel *log_label = new QLabel("Log: ");
	QLabel *rread = new QLabel("Data:");
	
	QPushButton *scan = new QPushButton("Scan 1Wire");
	scan->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *read_btn = new QPushButton("Read 1Wire");
	read_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *write_btn = new QPushButton("Write 1Wire");
	write_btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	
	QVBoxLayout *vlayout = new QVBoxLayout;
	QHBoxLayout *hlayout = new QHBoxLayout;
	QHBoxLayout *rom_layout = new QHBoxLayout;
	QHBoxLayout *pad_layout = new QHBoxLayout;
	QHBoxLayout *mem_layout = new QHBoxLayout;
	QHBoxLayout *file_input_layout = new QHBoxLayout;
	QHBoxLayout *dev_input_layout = new QHBoxLayout;
	
	msglog = new QTextEdit;
	msglog->setReadOnly(true);
	device_addr = new QLineEdit;
	
	dev_rom_read = new QLineEdit("0x33");

	rom_layout->addWidget(rread);
	rom_layout->addWidget(dev_rom_read);

	hlayout->addWidget(scan);
	hlayout->addWidget(read_btn);
	hlayout->addWidget(write_btn);
	dev_input_layout->addWidget(device_label);
	dev_input_layout->addWidget(device_addr);
	vlayout->addLayout(dev_input_layout);
	vlayout->addLayout(rom_layout);
	vlayout->addLayout(pad_layout);
	vlayout->addLayout(mem_layout);
	vlayout->addSpacing(10);
	vlayout->addLayout(hlayout);
	vlayout->addSpacing(50);
	vlayout->addWidget(log_label);
	vlayout->addWidget(msglog);
	
	setLayout(vlayout);
}

void OneWireGui::customEvent(QEvent *ev)
{
	if (static_cast<BPEventType>(ev->type()) == OneWireLogMsgEventType)
	{
		msglog->append(dynamic_cast<OneWireLogMsgEvent* >(ev)->msg);
	}
}

void OneWireGui::postMsgEvent(const char* msg)
{
	QString qmsg = QString(msg);
	QCoreApplication::sendEvent(this, new OneWireLogMsgEvent(qmsg));
}

