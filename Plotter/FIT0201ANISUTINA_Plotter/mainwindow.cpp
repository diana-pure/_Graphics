#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cassiniStep = 50;
    connect(ui->cassiniPlotWg, SIGNAL(wasSetFocus1(QPoint)), this, SLOT(setFocus1Value(QPoint)));
    connect(ui->cassiniPlotWg, SIGNAL(wasSetFocus2(QPoint)), this, SLOT(setFocus2Value(QPoint)));
    connect(ui->radiusSlider, SIGNAL(valueChanged(int)), ui->cassiniPlotWg, SLOT(setRadius(int)));
    ui->cassiniPlotWg->setStep(cassiniStep);
    ui->cassiniPlotWg->drawPlot();
    ui->curvePlotWg->drawPlot();
    connect(ui->curvePlotWg, SIGNAL(stepChanged(double)), ui->stepValue, SLOT(setNum(double)));
    connect(ui->curvePlotWg, SIGNAL(scaleChanged(double)), ui->scaleValue, SLOT(setNum(double)));
    connect(this, SIGNAL(windowWasResized(QSize)), ui->cassiniPlotWg, SLOT(drawPlot(QSize)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setFocus1Value(QPoint pnt)
{
    ui->foc1X->setNum(pnt.x() / static_cast<double>(cassiniStep));
    ui->foc1Y->setNum(pnt.y() / static_cast<double>(cassiniStep));
}
void MainWindow::setFocus2Value(QPoint pnt)
{
    ui->foc2X->setNum(pnt.x() / static_cast<double>(cassiniStep));
    ui->foc2Y->setNum(pnt.y() / static_cast<double>(cassiniStep));
}
void MainWindow::on_radiusSlider_valueChanged(int value)
{
    ui->radiusValue->setNum(value / static_cast<double>(cassiniStep));
}
void MainWindow::on_resetScaleButton_clicked()
{
    ui->curvePlotWg->setDefaults();
}
void MainWindow::resizeEvent(QResizeEvent *)
{
    //emit windowWasResized(size());
}
