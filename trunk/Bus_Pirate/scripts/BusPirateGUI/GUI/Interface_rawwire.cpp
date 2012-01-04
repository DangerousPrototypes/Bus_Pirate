#include <QtGui>
#include "BinMode.h"
#include "BPSettings.h"
#include "MainWin.h"
#include "Interface.h"
#include "Events.h"

RawWireGui::RawWireGui(MainWidgetFrame *p) : QWidget(p)
{
	QLabel *file_label = new QLabel("File: ");
	QLabel *device_label = new QLabel("Device: ");
	QLabel *log_label = new QLabel("Log: ");

	QPushButton *scan = new QPushButton("Scan RawWire");
	QPushButton *read_btn = new QPushButton("Read RawWire");
	QPushButton *write_btn = new QPushButton("Write RawWire");
	
	device_addr = new QLineEdit;
	file = new QLineEdit;
	msglog = new QTextEdit;
	msglog->setReadOnly(true);
	
	QVBoxLayout *vlayout = new QVBoxLayout;
	QHBoxLayout *hlayout = new QHBoxLayout;
	
	hlayout->addWidget(scan);
	hlayout->addWidget(read_btn);
	hlayout->addWidget(write_btn);
	
	vlayout->addWidget(file_label);
	vlayout->addWidget(file);
	vlayout->addWidget(device_label);
	vlayout->addWidget(device_addr);
	vlayout->addLayout(hlayout);
	vlayout->addSpacing(50);
	vlayout->addWidget(log_label);
	vlayout->addWidget(msglog);
	
	setLayout(vlayout);
}

void RawWireGui::customEvent(QEvent *ev)
{
	if (static_cast<BPEventType>(ev->type()) == RawWireLogMsgEventType)
	{
		msglog->append(dynamic_cast<RawWireLogMsgEvent* >(ev)->msg);
	}
}

void RawWireGui::postMsgEvent(const char* msg)
{
	QString qmsg = QString(msg);
	QCoreApplication::sendEvent(this, new RawWireLogMsgEvent(qmsg));
}

