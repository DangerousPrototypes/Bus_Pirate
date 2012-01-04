#include <QEvent>
#include "Events.h"

BPPortStatusMsgEvent::BPPortStatusMsgEvent(QString & msg) : 
	QEvent(static_cast<QEvent::Type>(BPPortStatusMsgEventType))
{
	this->msg = msg;
}

BPStatusMsgEvent::BPStatusMsgEvent(QString & msg) : 
	QEvent(static_cast<QEvent::Type>(BPStatusMsgEventType))
{
	this->msg = msg;
}

AsciiHexLogMsgEvent::AsciiHexLogMsgEvent(QString & msg) : 
	QEvent(static_cast<QEvent::Type>(AsciiHexLogMsgEventType))
{
	this->msg = msg;
}

SpiLogMsgEvent::SpiLogMsgEvent(QString & msg) : 
	QEvent(static_cast<QEvent::Type>(SpiLogMsgEventType))
{
	this->msg = msg;
}

I2CLogMsgEvent::I2CLogMsgEvent(QString & msg) : 
	QEvent(static_cast<QEvent::Type>(I2CLogMsgEventType))
{
	this->msg = msg;
}

OneWireLogMsgEvent::OneWireLogMsgEvent(QString & msg) : 
	QEvent(static_cast<QEvent::Type>(OneWireLogMsgEventType))
{
	this->msg = msg;
}

RawWireLogMsgEvent::RawWireLogMsgEvent(QString & msg) : 
	QEvent(static_cast<QEvent::Type>(RawWireLogMsgEventType))
{
	this->msg = msg;
}

JtagLogMsgEvent::JtagLogMsgEvent(QString & msg) :
	QEvent(static_cast<QEvent::Type>(JtagLogMsgEventType))
{
	this->msg = msg;
}