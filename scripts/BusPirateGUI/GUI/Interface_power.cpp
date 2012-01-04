#include <QtGui>
#include "BinMode.h"
#include "BPSettings.h"
#include "MainWin.h"
#include "Interface.h"
#include "Events.h"

PowerGui::PowerGui(MainWidgetFrame *parent) : QWidget(parent)
{
	this->parent = parent;

	QVBoxLayout *vlayout = new QVBoxLayout;

	QPushButton *bbio_mode = new QPushButton("Reset BBIO Mode");
	bbio_mode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *hard_reset = new QPushButton("Hardware Reset - Bus Pirate");
	hard_reset->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *userterm_reset = new QPushButton("User Terminal Reset");
	userterm_reset->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *enter_bbio = new QPushButton("Enter BBIO Mode");
	enter_bbio->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QPushButton *hiz_mode =  new QPushButton("Enable Hi-Z Power");
	hiz_mode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *normal_mode = new QPushButton("Enable Normal (3.3v) Power");
	normal_mode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *open_collector = new QPushButton("Enable Open Collector (5v) Power");
	open_collector->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QPushButton *short_test = new QPushButton("Short Self-Test");
	short_test->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *long_test = new QPushButton("Long Self-Test");
	long_test->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *fetch_buffer = new QPushButton("Fetch Buffer Contents");
	fetch_buffer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	connect(hiz_mode, SIGNAL(clicked()), this, SLOT(hiz_power_enable()));

	connect(bbio_mode, SIGNAL(clicked()), this, SLOT(resetBBIO()));
	connect(hard_reset, SIGNAL(clicked()), this, SLOT(hardReset()));
	connect(userterm_reset, SIGNAL(clicked()), this, SLOT(userTermReset()));
	connect(enter_bbio, SIGNAL(clicked()), this, SLOT(enterMode()));
	connect(fetch_buffer, SIGNAL(clicked()), this, SLOT(getBuffer()));

	vlayout->addWidget(bbio_mode);
	vlayout->addWidget(hard_reset);
	vlayout->addWidget(userterm_reset);
	vlayout->addWidget(enter_bbio);
	vlayout->addSpacing(50);
	vlayout->addWidget(hiz_mode);
	vlayout->addWidget(normal_mode);
	vlayout->addWidget(open_collector);
	vlayout->addSpacing(50);
	vlayout->addWidget(short_test);
	vlayout->addWidget(long_test);
	vlayout->addWidget(fetch_buffer);
	setLayout(vlayout);
}

void PowerGui::hiz_power_enable()
{
	qDebug() << "enable HiZ Power Mode";
	if (parent->bp->reset_bbio())
	{
		/* we are in bbio mode */
	} else {
		/* we are in the user terminal */
		parent->bp->reset_user_terminal();
		/* setup power in high-z mode here */
	}
}

void PowerGui::open_collector_power()
{
	qDebug() << "enable collector power";
}

void PowerGui::normal_power_enable()
{
	qDebug() << "enable normal power";
}

void PowerGui::hardReset()
{
	qDebug() << "Hard Reset";
	parent->bp->reset_bbio();
	parent->bp->reset_hardware();
}

void PowerGui::enterMode()
{
	qDebug() << "Enter Bit Banging Mode";
	parent->bp->enter_mode_bbio();
}

void PowerGui::userTermReset()
{
	qDebug() << "User Terminal Reset";
	parent->bp->reset_user_terminal();
}

void PowerGui::resetBBIO()
{
	qDebug() << "Reset BBIO Mode";
	parent->bp->reset_bbio();
}

void PowerGui::setupBusPirate()
{
	qDebug() << "Setup Bus Pirate";
	parent->bp->port_open();
	parent->bp->reset_bbio();
	parent->bp->reset_hardware();
	parent->bp->reset_user_terminal();
	parent->bp->enter_mode_bbio();	
}

void PowerGui::getBuffer()
{
	parent->bp->serial->flush();
	parent->bp->dumpBuffer();
	parent->bp->serial->flush();
}
