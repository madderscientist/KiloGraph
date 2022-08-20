#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	{
        QFile qss(":/style.css");
		qss.open(QFile::ReadOnly);
		a.setStyleSheet(qss.readAll());
		qss.close();
	}
	w.show();
	return a.exec();
}
