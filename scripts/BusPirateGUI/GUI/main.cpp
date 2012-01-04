#include <QtGui>

#include "MainWin.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	app.setApplicationName("QBusPirate");
	app.setOrganizationName("Sean Nelson");
	MainAppWindow *win = new MainAppWindow;
	win->show();
	return app.exec();
}
