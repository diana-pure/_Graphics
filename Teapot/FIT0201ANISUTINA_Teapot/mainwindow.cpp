#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <teapotprojectorwidget.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->segmentNumerSlider, SIGNAL(valueChanged(int)), ui->teapotWg, SLOT(changeSegmentNumber(int)));
    connect(ui->distanceSlider, SIGNAL(valueChanged(int)), ui->teapotWg, SLOT(changeDistance(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
     ui->teapotWg->setSceneSize(e->size());
}

void MainWindow::on_axisFlag_clicked()
{
    ui->teapotWg->axisControl(ui->axisFlag->isChecked());
}
