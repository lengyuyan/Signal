#include "ceway.h"
#include <QtWidgets/QApplication>
#include "qcustomplot.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Ceway w;
	w.show();
	return a.exec();
}
