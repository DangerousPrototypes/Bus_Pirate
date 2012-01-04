#ifndef __QMAIN_H
#define __QMAIN_H

#include <QtGui>

class SpiGui;
class JtagGui;
class I2CGui;
class OneWireGui;
class RawWireGui;
class RawTextGui;
class PowerGui;
class BBIOSettingsGui;
class BPSettingsGui;
class BinMode;
class BPSettings;
class MainAppWindow;
class MainWidgetFrame : public QWidget
{
Q_OBJECT
public:
	MainWidgetFrame(MainAppWindow *p);
	QTabWidget *tabs;
	BPSettings *cfg;
	BinMode *bp;
	SpiGui *spi;
	JtagGui *jtag;
	I2CGui *i2c;
	OneWireGui *onewire;
	RawWireGui *rawwire;
	RawTextGui *raw_text;
	PowerGui *power;
	BBIOSettingsGui *bbio;
	BPSettingsGui *settings;
	MainAppWindow *parent;
};

class MainAppWindow : public QMainWindow
{
protected:
	virtual void customEvent(QEvent *ev);
public:
	MainAppWindow();
	MainWidgetFrame *frame;
private:
	void createActions();
	void createMenus();
	QLabel *bpstatus;
};

#endif

