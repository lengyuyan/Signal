#include "ceway.h"
#include <opencv2\opencv.hpp>
#include "ui_ceway.h"
#include <iostream>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include "qcustomplot.h"
using namespace std;
using namespace cv;

typedef struct {
	double time;
	double time_Lap;
	double time_Edge;
	double adjust_z;
	double adjust_u;
	double adjust_v;
	vector<double> horizontal_lap;
	vector<double> vertical_lap;
}Running;
typedef struct{
	vector<double> horizontal_lap;
	vector<double> vertical_lap;
	vector<Point> horizontal_pt;
	vector<Point> vertical_pt;
	double time_Edge;
}HV_Lap;

Running Run(vector<string> filename, double deltaZ_p, double deltaZ_m);
vector<HV_Lap> Test(double num, vector<string> filename);
vector<string> imgnames;
vector<string> testnames;
double img_num[3];
vector<double> test_num;
int maxnum(QVector<double> a, int size);

Ceway::Ceway(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

Ceway::~Ceway()
{

}

void Ceway::on_pushButton_clicked(){
	double deltaZ_p;
	double deltaZ_m;
	deltaZ_p = img_num[0] - img_num[1];
	deltaZ_m = img_num[2] - img_num[1];
	Running Running;
	Running = Run(imgnames, deltaZ_p, deltaZ_m);

	ui.lineEdit_17->setText(QString::number(Running.time) + "s"); //Time_All
	ui.lineEdit_4->setText(QString::number(Running.adjust_z));  //Adjust_Z
	ui.lineEdit_5->setText(QString::number(Running.adjust_u));  //Adjust_U
	ui.lineEdit_6->setText(QString::number(Running.adjust_v));  //Adjust_V
	ui.lineEdit_15->setText(QString::number(Running.time_Edge) + "s");  //Time_Edge
	ui.lineEdit_16->setText(QString::number(Running.time_Lap) + "s");  //Time_Lap

	
	//绘制Left
	QVector<double> x(3), y(3);
	x[0] = deltaZ_p;    x[1] = 0;     x[2] = deltaZ_m;
	y[0] = Running.horizontal_lap[0];
	y[1] = Running.horizontal_lap[3];
	y[2] = Running.horizontal_lap[6];
	ui.customPlot->addGraph();
	ui.customPlot->graph()->addData(x, y);
	ui.customPlot->xAxis->setLabel("Delta_Z");
	ui.customPlot->yAxis->setLabel("Laplace");
	ui.customPlot->QCustomPlot::rescaleAxes();
	ui.customPlot->autoFillBackground();
	QPoint pos(10, 10);
	QMouseEvent click(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot, &click);
	QMouseEvent release(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot, &release);
	//绘制Center_x
	QVector<double> y1(3);
	y1[0] = Running.horizontal_lap[1];
	y1[1] = Running.horizontal_lap[4];
	y1[2] = Running.horizontal_lap[7];
	ui.customPlot_2->addGraph();
	ui.customPlot_2->graph()->addData(x, y1);
	ui.customPlot_2->xAxis->setLabel("Delta_Z");
	ui.customPlot_2->yAxis->setLabel("Laplace");
	ui.customPlot_2->QCustomPlot::rescaleAxes();
	QMouseEvent click2(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_2, &click2);
	QMouseEvent release2(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_2, &release2);
	//绘制Right
	QVector<double> y2(3);
	y2[0] = Running.horizontal_lap[2];
	y2[1] = Running.horizontal_lap[5];
	y2[2] = Running.horizontal_lap[8];
	ui.customPlot_3->addGraph();
	ui.customPlot_3->graph()->addData(x, y2);
	ui.customPlot_3->xAxis->setLabel("Delta_Z");
	ui.customPlot_3->yAxis->setLabel("Laplace");
	ui.customPlot_3->QCustomPlot::rescaleAxes();
	QMouseEvent click3(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_3, &click3);
	QMouseEvent release3(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_3, &release3);
	//绘制Top
	QVector<double> y3(3);
	y3[0] = Running.vertical_lap[0];
	y3[1] = Running.vertical_lap[3];
	y3[2] = Running.vertical_lap[6];
	ui.customPlot_4->addGraph();
	ui.customPlot_4->graph()->addData(x, y3);
	ui.customPlot_4->xAxis->setLabel("Delta_Z");
	ui.customPlot_4->yAxis->setLabel("Laplace");
	ui.customPlot_4->QCustomPlot::rescaleAxes();
	QMouseEvent click4(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_4, &click4);
	QMouseEvent release4(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_4, &release4);
	//绘制Center_y
	QVector<double> y4(3);
	y4[0] = Running.vertical_lap[1];
	y4[1] = Running.vertical_lap[4];
	y4[2] = Running.vertical_lap[7];
	ui.customPlot_5->addGraph();
	ui.customPlot_5->graph()->addData(x, y4);
	ui.customPlot_5->xAxis->setLabel("Delta_Z");
	ui.customPlot_5->yAxis->setLabel("Laplace");
	ui.customPlot_5->QCustomPlot::rescaleAxes();
	QMouseEvent click5(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_5, &click5);
	QMouseEvent release5(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_5, &release5);
	//绘制Bottom
	QVector<double> y5(3);
	y5[0] = Running.vertical_lap[2];
	y5[1] = Running.vertical_lap[5];
	y5[2] = Running.vertical_lap[8];
	ui.customPlot_6->addGraph();
	ui.customPlot_6->graph()->addData(x, y5);
	ui.customPlot_6->xAxis->setLabel("Delta_Z");
	ui.customPlot_6->yAxis->setLabel("Laplace");
	ui.customPlot_6->QCustomPlot::rescaleAxes();
	QMouseEvent click11(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_6, &click11);
	QMouseEvent release11(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_6, &release11);
}
void Ceway::on_pushButton_2_clicked(){
	ui.lineEdit_4->clear();
	ui.lineEdit_5->clear();
	ui.lineEdit_6->clear();
	ui.lineEdit_15->clear();
	ui.lineEdit_16->clear();
	ui.lineEdit_17->clear();
	ui.lineEdit_7->clear();
	ui.lineEdit_8->clear();
	ui.lineEdit_9->clear();
	ui.lineEdit_18->clear();
	ui.lineEdit_19->clear();
	ui.lineEdit_20->clear();
	ui.customPlot->graph()->clearData();
	ui.customPlot_2->graph()->clearData();
	ui.customPlot_3->graph()->clearData();
	ui.customPlot_4->graph()->clearData();
	ui.customPlot_5->graph()->clearData();
	ui.customPlot_6->graph()->clearData();
	QPoint pos2(10, 10);
	QMouseEvent click6(QEvent::MouseButtonPress, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot, &click6);
	QMouseEvent release6(QEvent::MouseButtonRelease, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot, &release6);
	QMouseEvent click7(QEvent::MouseButtonPress, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_2, &click7);
	QMouseEvent release7(QEvent::MouseButtonRelease, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_2, &release7);
	QMouseEvent click8(QEvent::MouseButtonPress, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_3, &click8);
	QMouseEvent release8(QEvent::MouseButtonRelease, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_3, &release8);
	QMouseEvent click9(QEvent::MouseButtonPress, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_4, &click9);
	QMouseEvent release9(QEvent::MouseButtonRelease, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_4, &release9);
	QMouseEvent click10(QEvent::MouseButtonPress, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_5, &click10);
	QMouseEvent release10(QEvent::MouseButtonRelease, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_5, &release10);
	QMouseEvent click12(QEvent::MouseButtonPress, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_6, &click12);
	QMouseEvent release12(QEvent::MouseButtonRelease, pos2, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_6, &release12);

	imgnames = {};
	testnames = {};
	test_num = {};
}
void Ceway::on_pushButton_3_clicked(){
	int num = 3;
	for (int i = 0; i < num; i++){
		QString filename = QFileDialog::getOpenFileName(this, tr("Select Image"), ".", tr("Image Files(*.png *.jpg *.bmp)"));
		string filename2;
		filename2 = filename.toStdString();
		imgnames.push_back(filename2);

		QFileInfo fi; QString temp;  int index;
		fi = QFileInfo(filename);
		temp = fi.fileName();
		index = temp.lastIndexOf(".");
		temp.truncate(index);
		img_num[i] = temp.toDouble();
	}
}

void Ceway::on_pushButton_4_clicked(){
	int num;
	num = ui.spinBox->value();
	for (int i = 0; i < num; i++){
		QString filename = QFileDialog::getOpenFileName(this, tr("Select Image"), ".", tr("Image Files(*.png *.jpg *.bmp)"));
		string filename2;
		filename2 = filename.toStdString();
		testnames.push_back(filename2);

		QFileInfo fi; QString temp;  int index;
		fi = QFileInfo(filename);
		temp = fi.fileName();
		index = temp.lastIndexOf(".");
		temp.truncate(index);
		test_num.push_back(temp.toDouble());
	}
}
void Ceway::on_pushButton_5_clicked(){
	vector<HV_Lap> lap;
	int num = ui.spinBox->value();
	lap = Test(num, testnames);
	QVector<double> x(num), left(num), center_x(num), right(num), top(num), center_y(num), bottom(num);
	for (int i = 0; i < num;i++){
		x[i] = test_num[i];
		left[i] = lap[i].horizontal_lap[0];
		center_x[i] = lap[i].horizontal_lap[1];
		right[i] = lap[i].horizontal_lap[2];
		top[i] = lap[i].vertical_lap[0];
		center_y[i] = lap[i].vertical_lap[1];
		bottom[i] = lap[i].vertical_lap[2];
	}
	ui.lineEdit_9->setText(QString::number(x[maxnum(left, num)]));
	ui.lineEdit_7->setText(QString::number(x[maxnum(center_x, num)]));
	ui.lineEdit_8->setText(QString::number(x[maxnum(right, num)]));
	ui.lineEdit_18->setText(QString::number(x[maxnum(top, num)]));
	ui.lineEdit_19->setText(QString::number(x[maxnum(center_y, num)]));
	ui.lineEdit_20->setText(QString::number(x[maxnum(bottom, num)]));
	//Left
	ui.customPlot->addGraph();
	ui.customPlot->graph()->addData(x, left);
	ui.customPlot->xAxis->setLabel("Z");
	ui.customPlot->yAxis->setLabel("Laplace");
	ui.customPlot->QCustomPlot::rescaleAxes();
	ui.customPlot->autoFillBackground();
	QPoint pos(10, 10);
	QMouseEvent click(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot, &click);
	QMouseEvent release(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot, &release);
	//Center_x
	ui.customPlot_2->addGraph();
	ui.customPlot_2->graph()->addData(x, center_x);
	ui.customPlot_2->xAxis->setLabel("Z");
	ui.customPlot_2->yAxis->setLabel("Laplace");
	ui.customPlot_2->QCustomPlot::rescaleAxes();
	QMouseEvent click2(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_2, &click2);
	QMouseEvent release2(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_2, &release2);
	//Right
	ui.customPlot_3->addGraph();
	ui.customPlot_3->graph()->addData(x, right);
	ui.customPlot_3->xAxis->setLabel("Z");
	ui.customPlot_3->yAxis->setLabel("Laplace");
	ui.customPlot_3->QCustomPlot::rescaleAxes();
	QMouseEvent click3(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_3, &click3);
	QMouseEvent release3(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_3, &release3);
	//Top
	ui.customPlot_4->addGraph();
	ui.customPlot_4->graph()->addData(x, top);
	ui.customPlot_4->xAxis->setLabel("Z");
	ui.customPlot_4->yAxis->setLabel("Laplace");
	ui.customPlot_4->QCustomPlot::rescaleAxes();
	QMouseEvent click4(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_4, &click4);
	QMouseEvent release4(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_4, &release4);
	//Center_y
	ui.customPlot_5->addGraph();
	ui.customPlot_5->graph()->addData(x, center_y);
	ui.customPlot_5->xAxis->setLabel("Z");
	ui.customPlot_5->yAxis->setLabel("Laplace");
	ui.customPlot_5->QCustomPlot::rescaleAxes();
	QMouseEvent click5(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_5, &click5);
	QMouseEvent release5(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_5, &release5);
	//Bottom
	ui.customPlot_6->addGraph();
	ui.customPlot_6->graph()->addData(x, bottom);
	ui.customPlot_6->xAxis->setLabel("Z");
	ui.customPlot_6->yAxis->setLabel("Laplace");
	ui.customPlot_6->QCustomPlot::rescaleAxes();
	QMouseEvent click11(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_6, &click11);
	QMouseEvent release11(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(ui.customPlot_6, &release11);
}


int maxnum(QVector<double> a, int size)
{
	double max = 0;
	int maxnum = 0;
	for (int i = 0; i < size; i++)
	{
		if (max < a[i])
		{
			max = a[i];
			maxnum = i;
		}
	}
	return maxnum;
}