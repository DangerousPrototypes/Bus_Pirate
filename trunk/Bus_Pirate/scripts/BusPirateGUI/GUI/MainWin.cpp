#include <QtGui>
#include "Interface.h"
#include "BPSettings.h"
#include "BinMode.h"
#include "MainWin.h"
#include "Events.h"
#include "configure.h"

MainAppWindow::MainAppWindow()
{
	/* QSplashScreen */
	createActions();
	createMenus();
	setStatusBar(new QStatusBar);
	statusBar()->showMessage("Ready");
	bpstatus = new QLabel("Bus Pirate Closed");
	statusBar()->addPermanentWidget(bpstatus);
	frame = new MainWidgetFrame(this);
	setCentralWidget(frame);
}

void MainAppWindow::customEvent(QEvent *ev)
{
	if (static_cast<BPEventType>(ev->type()) == BPStatusMsgEventType)
	{
		statusBar()->showMessage(dynamic_cast<BPStatusMsgEvent* >(ev)->msg);
	}
	else if (static_cast<BPEventType>(ev->type()) == BPPortStatusMsgEventType)
	{
		bpstatus->setText(dynamic_cast<BPPortStatusMsgEvent *>(ev)->msg);
	}
}

void MainAppWindow::createActions(){}
void MainAppWindow::createMenus(){}

MainWidgetFrame::MainWidgetFrame(MainAppWindow *p) : QWidget(p)
{
	parent=p;
	tabs = new QTabWidget(p);
	cfg = new BPSettings;
	settings = new BPSettingsGui(this);
	bp = new BinMode(this);

	connect(qApp, SIGNAL(lastWindowClosed()), settings, SLOT(SaveSettings()));

#if ENABLE_SPI
	spi = new SpiGui(this);
	tabs->addTab(spi, "SPI");
#endif
#if ENABLE_I2C
	i2c = new I2CGui(this);
	tabs->addTab(i2c, "I2C");
#endif
#if ENABLE_1WIRE
	onewire = new OneWireGui(this);
	tabs->addTab(onewire, "1Wire");
#endif
#if ENABLE_RAWWIRE
	rawwire = new RawWireGui(this);
	tabs->addTab(rawwire, "Raw 2/3-Wire");
#endif
#if ENABLE_ASCII
	raw_text = new RawTextGui(this);
	tabs->addTab(raw_text, "Raw Text");
#endif
#if ENABLE_JTAG
	jtag = new JtagGui(this);
	tabs->addTab(jtag, "JTAG");
#endif
	//power = new PowerGui(this);
	//tabs->addTab(power, "Bus Pirate");
	bbio = new BBIOSettingsGui(this);
	tabs->addTab(bbio, "BBIO Settings");
	tabs->addTab(settings, "Settings");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabs);
	setLayout(mainLayout);
	setWindowTitle("BusPirateGUI");
}

