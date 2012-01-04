#ifndef __EVENT_H
#define __EVENT_H

#include <QEvent>
#include <QString>

enum BPEventType
{
	BPStatusMsgEventType = QEvent::User,
	BPPortStatusMsgEventType,
	AsciiHexLogMsgEventType,
	SpiLogMsgEventType,
	I2CLogMsgEventType,
	OneWireLogMsgEventType,
	RawWireLogMsgEventType,
	JtagLogMsgEventType,
};

class AsciiHexLogMsgEvent : public QEvent
{
public:
	QString msg;
	AsciiHexLogMsgEvent(QString & msg);
};

class SpiLogMsgEvent : public QEvent
{
public:
	QString msg;
	SpiLogMsgEvent(QString & msg);
};

class I2CLogMsgEvent : public QEvent
{
public:
	QString msg;
	I2CLogMsgEvent(QString & msg);
};

class OneWireLogMsgEvent : public QEvent
{
public:
	QString msg;
	OneWireLogMsgEvent(QString & msg);
};

class BPStatusMsgEvent : public QEvent
{
public:
	QString msg;
	BPStatusMsgEvent(QString & msg);
};

class BPPortStatusMsgEvent : public QEvent
{
public:
	QString msg;
	BPPortStatusMsgEvent(QString & msg);
};

class RawWireLogMsgEvent : public QEvent
{
public:
	QString msg;
	RawWireLogMsgEvent(QString & msg);
};

class JtagLogMsgEvent : public QEvent
{
public:
	QString msg;
	JtagLogMsgEvent(QString & msg);
};

#endif

