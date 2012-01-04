#ifndef __BPSETTINGS_H
#define __BPSETTINGS_H

#include <QtGui>

class BinMode;
class MainWidgetFrame;

class BPSettings : public QSettings
{
Q_OBJECT
public:
	BPSettings();
	~BPSettings();
	QSettings *m_cfg;
	QString serial_port_name;
	int baud_rate;
	int databits;
	int stopbits;
	int parity;
	int flowctrl;
public slots:
	void Save();
	void Load();
};

class BBIOSettingsGui : public QWidget
{
public:
	BBIOSettingsGui(MainWidgetFrame *parent);
private:
	MainWidgetFrame *parent;
	/* peripherials */
	QCheckBox *power_supplies;
	QCheckBox *voltage_pullups;
	QCheckBox *aux_enable;
	QCheckBox *cs_enable;
	
	/* spi */
	QComboBox *spi_speed;
	QComboBox *spi_output;
	QComboBox *spi_ckp_idle;
	QComboBox *spi_cke_edge;
	QComboBox *spi_smp_sample;

	/* i2c */
	QComboBox *i2c_speed;
	
	/* uart */
	QComboBox *uart_speed;
	QComboBox *uart_output;
	QComboBox *uart_dataparity;
	QComboBox *uart_stop;
	QComboBox *uart_rx_polarity;
	
	/* rawwire */
	QComboBox *rawwire_speed;
	QComboBox *rawwire_output;
	QComboBox *rawwire_protocol;
	QComboBox *rawwire_bitorder;
	
};

class BPSettingsGui : public QWidget
{
Q_OBJECT
public:
	BPSettingsGui(MainWidgetFrame *parent);
	~BPSettingsGui();
	void CreateBaudMap(void);
	QLineEdit *s_port;
	QComboBox *s_baud;
	QComboBox *s_databits;
	QComboBox *s_stopbits;
	QComboBox *s_parity;
	QComboBox *s_flow;
	QMap<QString, int> *usable_baud_rate;
	MainWidgetFrame *parent;
	void setConfigSettings();
	//void setupBusPirate();
public slots:
	void SaveSettings();
	void openPort();
	void closePort();
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
};

#endif

