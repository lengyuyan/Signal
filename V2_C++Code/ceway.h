#ifndef CEWAY_H
#define CEWAY_H

#include <QtWidgets/QMainWindow>
#include "ui_ceway.h"


class Ceway : public QMainWindow
{
	Q_OBJECT

public:
	Ceway(QWidget *parent = 0);
	~Ceway();

private slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_4_clicked();
	void on_pushButton_5_clicked();
private:
	Ui::CewayClass ui;
};

#endif // CEWAY_H
