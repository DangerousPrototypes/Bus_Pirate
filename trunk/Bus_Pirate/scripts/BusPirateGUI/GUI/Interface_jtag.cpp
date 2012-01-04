#include <QtGui>
#include "BinMode.h"
#include "BPSettings.h"
#include "MainWin.h"
#include "Interface.h"
#include "Events.h"

JtagGui::JtagGui(MainWidgetFrame *parent) : QWidget(parent)
{
	this->parent = parent;
}

void JtagGui::customEvent(QEvent *ev)
{
	if (static_cast<BPEventType>(ev->type()) == JtagLogMsgEventType)
	{
		msglog->append(dynamic_cast<JtagLogMsgEvent* >(ev)->msg);
	}
}

void JtagGui::postMsgEvent(const char* msg)
{
	QString qmsg = QString(msg);
	QCoreApplication::sendEvent(this, new JtagLogMsgEvent(qmsg));
}

