#include <QtGui>
#include "qextserialport/qextserialport.h"
#include "MainWin.h"
#include "BPSettings.h"
#include "Events.h"
#include "BinMode.h"

BPSettingsGui::BPSettingsGui(MainWidgetFrame *parent) : QWidget(parent)
{
	this->parent=parent;

	QHBoxLayout *m_layout = new QHBoxLayout;

	CreateBaudMap();
	QStringList baud_list;
	baud_list << "300" << "600" << "1200" << "2400" << "4800" << "9600" 
		<< "19200" << "38400" << "57600" << "115200";

	QStringList databits_list;
	databits_list << "5" << "6" << "7" << "8";

	QStringList parity_list;
	parity_list << "NONE" << "ODD" << "EVEN" << "SPACE"; 
	
	QStringList stopbits_list;
	stopbits_list << "1" << "2";

	QStringList flowctrl_list;
	flowctrl_list << "OFF" << "HARDWARE" << "SOFTWARE";


	QLabel *s_port_label = new QLabel("Serial Port: ");
	QLabel *s_baud_label = new QLabel("Port Baud Rate: ");
	QLabel *s_databits_label = new QLabel("Data bits: ");
	QLabel *s_stopbits_label = new QLabel("Stop bits: ");
	QLabel *s_parity_label = new QLabel("Parity: ");
	QLabel *s_flow_label = new QLabel("Flow Control: ");

	QPushButton *open = new QPushButton("Open BusPirate");
	open->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *close = new QPushButton("Close BusPirate");
	close->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	s_port = new QLineEdit(parent->cfg->serial_port_name);
	s_port->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	s_baud = new QComboBox;
	s_baud->addItems(baud_list);
	s_baud->setCurrentIndex(s_baud->findText(usable_baud_rate->key(parent->cfg->baud_rate)));
	s_baud->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	s_databits = new QComboBox;
	s_databits->addItems(databits_list);
	s_databits->setCurrentIndex(parent->cfg->databits);
	s_databits->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	s_stopbits = new QComboBox;
	s_stopbits->addItems(stopbits_list);
	s_stopbits->setCurrentIndex(parent->cfg->stopbits);
	s_stopbits->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	s_parity = new QComboBox;
	s_parity->addItems(parity_list);
	s_parity->setCurrentIndex(parent->cfg->parity);
	s_parity->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	s_flow = new QComboBox;
	s_flow->addItems(flowctrl_list);
	s_flow->setCurrentIndex(parent->cfg->flowctrl);
	s_flow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QVBoxLayout *vlayout = new QVBoxLayout;

	connect(open, SIGNAL(clicked()), this, SLOT(openPort()));
	connect(close, SIGNAL(clicked()), this, SLOT(closePort()));

	vlayout->addWidget(s_port_label);
	vlayout->addWidget(s_port);
	vlayout->addWidget(s_baud_label);
	vlayout->addWidget(s_baud);
	vlayout->addWidget(s_databits_label);
	vlayout->addWidget(s_databits);
	vlayout->addWidget(s_stopbits_label);
	vlayout->addWidget(s_stopbits);
	vlayout->addWidget(s_parity_label);
	vlayout->addWidget(s_parity);
	vlayout->addWidget(s_flow_label);
	vlayout->addWidget(s_flow);
	vlayout->addSpacing(50);
	vlayout->addWidget(open);
	vlayout->addWidget(close);

/* Power */
	QVBoxLayout *vlayout2 = new QVBoxLayout;

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
	QPushButton *normal_mode = new QPushButton("Enable 3.3v Power");
	normal_mode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPushButton *open_collector = new QPushButton("Enable 5v Power");
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

	vlayout2->addWidget(bbio_mode);
	vlayout2->addWidget(hard_reset);
	vlayout2->addWidget(userterm_reset);
	vlayout2->addWidget(enter_bbio);
	vlayout2->addSpacing(50);
	vlayout2->addWidget(hiz_mode);
	vlayout2->addWidget(normal_mode);
	vlayout2->addWidget(open_collector);
	vlayout2->addSpacing(50);
	vlayout2->addWidget(short_test);
	vlayout2->addWidget(long_test);
	vlayout2->addWidget(fetch_buffer);

	m_layout->addLayout(vlayout);
	m_layout->addLayout(vlayout2);
	setLayout(m_layout);
	//setupBusPirate();
}


void BPSettingsGui::hiz_power_enable()
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

void BPSettingsGui::open_collector_power()
{
	qDebug() << "enable collector power";
}

void BPSettingsGui::normal_power_enable()
{
	qDebug() << "enable normal power";
}

void BPSettingsGui::hardReset()
{
	qDebug() << "Hard Reset";
	parent->bp->reset_bbio();
	parent->bp->reset_hardware();
}

void BPSettingsGui::enterMode()
{
	qDebug() << "Enter Bit Banging Mode";
	parent->bp->enter_mode_bbio();
}

void BPSettingsGui::userTermReset()
{
	qDebug() << "User Terminal Reset";
	parent->bp->reset_user_terminal();
}

void BPSettingsGui::resetBBIO()
{
	qDebug() << "Reset BBIO Mode";
	parent->bp->reset_bbio();
}

void BPSettingsGui::getBuffer()
{
	parent->bp->serial->flush();
	parent->bp->dumpBuffer();
	parent->bp->serial->flush();
}

BPSettingsGui::~BPSettingsGui()
{
	SaveSettings();
}

void BPSettingsGui::SaveSettings()
{
	qDebug() << "Save Port Settings";
	parent->cfg->serial_port_name = s_port->text();
	parent->cfg->baud_rate = usable_baud_rate->value(s_baud->currentText(), BAUD115200);
	parent->cfg->databits = s_databits->currentIndex();
	parent->cfg->stopbits = s_stopbits->currentIndex();
	parent->cfg->parity = s_parity->currentIndex();
	parent->cfg->flowctrl = s_flow->currentIndex();
	parent->cfg->Save();
}

void BPSettingsGui::setConfigSettings()
{
	qDebug() << "set Port Settings";
	parent->bp->serial->setPortName(s_port->text());
	parent->bp->serial->setBaudRate((BaudRateType)usable_baud_rate->value(s_baud->currentText(), BAUD115200)); //BAUD115200
	parent->bp->serial->setDataBits((DataBitsType)s_databits->currentIndex());  //DATA_8
	parent->bp->serial->setStopBits((StopBitsType)s_stopbits->currentIndex());  //STOP_1
	parent->bp->serial->setParity((ParityType)s_parity->currentIndex());      //PAR_NONE
	parent->bp->serial->setFlowControl((FlowType)s_flow->currentIndex());      //FLOW_OFF
}

void BPSettingsGui::openPort()
{
	QString qmsg = QString("Bus Pirate Ready");
	if (parent->bp->port_open())
		QCoreApplication::sendEvent(parent->parent, new BPPortStatusMsgEvent(qmsg));
}

void BPSettingsGui::closePort()
{
	QString qmsg = QString("Bus Pirate Closed");
	parent->bp->port_close();
	QCoreApplication::sendEvent(parent->parent, new BPPortStatusMsgEvent(qmsg));
}

/*void BPSettingsGui::setupBusPirate()
{
	openPort();
	if (!parent->bp->reset_bbio())
		parent->bp->enter_mode_bbio();	
}*/

void BPSettingsGui::setupBusPirate()
{
	qDebug() << "Setup Bus Pirate";
	parent->bp->port_open();
	parent->bp->reset_bbio();
	parent->bp->reset_hardware();
	parent->bp->reset_user_terminal();
	parent->bp->enter_mode_bbio();	
}

void BPSettingsGui::CreateBaudMap(void)
{
	/*
    BAUD300,
    BAUD600,
    BAUD1200,
    BAUD2400,
    BAUD4800,
    BAUD9600,
    BAUD19200,
    BAUD38400,
    BAUD57600,
    BAUD115200,
    */
	usable_baud_rate = new QMap<QString, int>();
	usable_baud_rate->insert("300", BAUD300);
	usable_baud_rate->insert("600", BAUD600);
	usable_baud_rate->insert("1200", BAUD1200);
	usable_baud_rate->insert("2400", BAUD2400);
	usable_baud_rate->insert("4800", BAUD4800);
	usable_baud_rate->insert("9600", BAUD9600);
	usable_baud_rate->insert("19200", BAUD19200);
	usable_baud_rate->insert("38400", BAUD38400);
	usable_baud_rate->insert("57600", BAUD57600);
	usable_baud_rate->insert("115200", BAUD115200);
}

BPSettings::BPSettings() : QSettings("./rc_buspirate", QSettings::IniFormat)
{
	Load();
}

BPSettings::~BPSettings()
{
	Save();
}

void BPSettings::Save()
{
	qDebug() << "Saving Config File";
	setValue("/serial_port/name", serial_port_name);
	setValue("/serial_port/baud_rate", baud_rate);
	setValue("/serial_port/databits", databits);
	setValue("/serial_port/stopbits", stopbits);
	setValue("/serial_port/parity", parity);
	setValue("/serial_port/flowctrl", flowctrl);
}

void BPSettings::Load()
{
	qDebug() << "Loading Config File";
	serial_port_name = value("/serial_port/name", QString("/dev/bus_pirate")).toString();
	baud_rate = value("/serial_port/baud_rate", BAUD115200).toInt();
	databits = value("/serial_port/databits", DATA_8).toInt();
	stopbits = value("/serial_port/stopbits", STOP_1).toInt();
	parity = value("/serial_port/parity", PAR_NONE).toInt();
	flowctrl = value("/serial_port/flowctrl", FLOW_OFF).toInt();
	//qDebug() << serial_port_name;
}

BBIOSettingsGui::BBIOSettingsGui(MainWidgetFrame *parent)
{
	this->parent = parent;

	QGridLayout *m_layout = new QGridLayout;
	/* Peripherial Enables */
	QLabel *peripherials = new QLabel("Peripherials");
	m_layout->addWidget(peripherials, 0, 0);
	power_supplies = new QCheckBox("Enable Power Supplies", this);
	power_supplies->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(power_supplies, 1, 0);
	voltage_pullups = new QCheckBox("Enable Vpullups", this);
	voltage_pullups->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(voltage_pullups, 1, 1);
	aux_enable = new QCheckBox("Enable AUX", this);
	aux_enable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(aux_enable, 1, 2);
	cs_enable = new QCheckBox("Enable CS", this);
	cs_enable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(cs_enable, 2, 0);

	/* SPI config */
	/*
	* SPI speed - 
	* 000=30kHz, 001=125kHz, 010=250kHz, 011=1MHz, 100=2MHz, 101=2.6MHz, 
	* 110=4MHz, 111=8MHz
	*/
	/*
	QLabel *spi_label = new QLabel("SPI");
	m_layout->addWidget(spi_label, 3, 0);
	spi_speed = new QComboBox(this);
	spi_speed->addItems(QStringList()<<"30kHz"<<"125kHz");
	spi_speed->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(spi_speed, 4, 0);
	spi_output = new QComboBox(this);
	spi_output->addItems(QStringList()<<"HiZ"<<"3.3V");
	spi_output->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(spi_output, 4, 1);
	spi_ckp_idle = new QComboBox(this);
	spi_ckp_idle->addItems(QStringList()<<"Low"<<"High");
	spi_ckp_idle->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(spi_ckp_idle, 4, 2);
	spi_cke_edge = new QComboBox(this);
	spi_cke_edge->addItems(QStringList()<<"idle to active"<<"active to idle");
	spi_cke_edge->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(spi_cke_edge, 5, 0);
	spi_smp_sample = new QComboBox(this);
	spi_smp_sample->addItems(QStringList()<<"Middle"<<"End");
	spi_smp_sample->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(spi_smp_sample, 5, 1);
	*/

	/* I2C config */
	QLabel *i2c_label = new QLabel("I2C");
	m_layout->addWidget(i2c_label, 6, 0);
	i2c_speed = new QComboBox(this);
	i2c_speed->addItems(QStringList()<<"5KHz"<<"50KHz");
	i2c_speed->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(i2c_speed, 7, 0);
	
	/* UART config */
	/*
	*  Set the UART at a preconfigured speed value: 
	* 0000=300, 0001=1200, 0010=2400, 0011=4800, 0100=9600, 0101=19200,
	* 0110=31250 (MIDI), 0111=38400, 1000=57600, 1010=115200
	*/
	QLabel *uart_label = new QLabel("UART", this);
	m_layout->addWidget(uart_label, 8, 0);
	uart_speed = new QComboBox(this);
	uart_speed->addItems(QStringList()<<"300"<<"1200");
	uart_speed->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(uart_speed, 9, 0);
	uart_output = new QComboBox(this);
	uart_output->addItems(QStringList()<<"HiZ"<<"3.3V");
	uart_output->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(uart_output, 9, 1);
	uart_dataparity = new QComboBox(this);
	uart_dataparity->addItems(QStringList()<<"8/N"<<"8/E"<<"8/O"<<"9/N");
	uart_dataparity->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(uart_dataparity, 9, 2);
	uart_stop = new QComboBox(this);
	uart_stop->addItems(QStringList()<<"1"<<"2");
	uart_stop->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(uart_stop, 10, 0);
	uart_rx_polarity = new QComboBox(this);
	uart_rx_polarity->addItems(QStringList()<<"idle High"<<"idle Low");
	uart_rx_polarity->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(uart_rx_polarity, 10, 1);
	
	/* rawwire config */
	QLabel *rawire_label = new QLabel("RawWire", this);
	m_layout->addWidget(rawire_label, 11, 0);
	rawwire_speed = new QComboBox(this);
	rawwire_speed->addItems(QStringList()<<"5KHz"<<"50KHz");
	rawwire_speed->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(rawwire_speed, 12, 0);
	rawwire_output = new QComboBox(this);
	rawwire_output->addItems(QStringList()<<"HiZ"<<"3.3V");
	rawwire_output->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(rawwire_output, 12, 1);
	rawwire_protocol = new QComboBox(this);
	rawwire_protocol->addItems(QStringList()<<"2-Wire"<<"3-Wire");
	rawwire_protocol->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(rawwire_protocol, 12, 2);
	rawwire_bitorder = new QComboBox(this);
	rawwire_bitorder->addItems(QStringList()<<"MSB"<<"LSB");
	rawwire_bitorder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	m_layout->addWidget(rawwire_bitorder, 13, 0);

	setLayout(m_layout);
}
