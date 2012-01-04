#include <QtGui>
#include "BinMode.h"
#include "BPSettings.h"
#include "MainWin.h"
#include "Interface.h"
#include "Events.h"

/* Interface: Raw Ascii Text */
RawTextGui::RawTextGui(MainWidgetFrame *parent) : QWidget(parent)
{
	this->parent = parent;

	QVBoxLayout *vlayout = new QVBoxLayout;
	QLabel *file_label = new QLabel("File: ");
	raw_file = new QLineEdit("test_hex_ascii.txt");
	QPushButton *button = new QPushButton("Run");
	QLabel *log_label = new QLabel("Log: ");
	msglog = new QTextEdit;
	
	connect(button, SIGNAL(clicked()), this, SLOT(ExecuteFile()));

	vlayout->addWidget(file_label);
	vlayout->addWidget(raw_file);
	vlayout->addWidget(button);
	vlayout->addSpacing(50);
	vlayout->addWidget(log_label);
	vlayout->addWidget(msglog);
	
	setLayout(vlayout);
}

void RawTextGui::customEvent(QEvent *ev)
{
	if (static_cast<BPEventType>(ev->type()) == AsciiHexLogMsgEventType)
	{
		msglog->append(dynamic_cast<AsciiHexLogMsgEvent* >(ev)->msg);
	}
}

void RawTextGui::postMsgEvent(const char* msg)
{
	QString qmsg = QString(msg);
	QCoreApplication::sendEvent(this, new AsciiHexLogMsgEvent(qmsg));
}

void RawTextGui::ExecuteFile()
{
	bool ok;
	uint i=0;
	QByteArray ascii_hex_val;
	QByteArray response;
	QByteArray byte;
	QList<QByteArray> byte_list;
	QString qmsg_start ="Exec: ASCII Script...";
	QString qmsg_success = "Exec: ASCII Script...Success!";
	QString str = "%1 | raw bytes text: %2 tx: %3 rx: %4";
	QString qmsg;
	uint d;

	QFile file(raw_file->text());

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(qmsg_start));

	ascii_hex_val = file.readAll().trimmed();
	byte_list = ascii_hex_val.replace('\n', ' ').split(' ');

	while(!byte_list.isEmpty())
	{
		byte = byte_list.takeFirst();
		d = byte.trimmed().toUShort(&ok, 0);
		response = parent->bp->command(d);
		qmsg = str.arg(i++, 3, 10, QChar('0')).arg(byte.data())
			.arg(d, 3, 10, QChar('0')).arg(response.toHex().data());
		QCoreApplication::sendEvent(this, new AsciiHexLogMsgEvent(qmsg));
	}
	QCoreApplication::sendEvent(parent->parent, new BPStatusMsgEvent(qmsg_success));
}

