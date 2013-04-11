#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <teapotprojectorwidget.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    files_names = QStringList();
    loadListOfFiles();
    connect(ui->segmentNumerSlider, SIGNAL(valueChanged(int)), ui->teapotWg, SLOT(changeSegmentNumber(int)));
    connect(ui->distanceSlider, SIGNAL(valueChanged(int)), ui->teapotWg, SLOT(changeDistance(int)));
    connect(ui->distanceToPlaneSlider, SIGNAL(valueChanged(int)), ui->teapotWg, SLOT(changeDistanceToPlane(int)));
    connect(ui->modelsList, SIGNAL(currentTextChanged(QString)), ui->teapotWg, SLOT(updateProjection(QString)));
    connect(ui->teapotWg, SIGNAL(moveCamera(double)), ui->deltaRmoveValueLbl, SLOT(setNum(double)));
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

void MainWindow::loadListOfFiles() {
    files_names.append("teapot.bpt");
    files_names.append("teapottall.bpt");
    files_names.append("teacup.bpt");
    files_names.append("teaspoon.bpt");
    ui->modelsList->addItems(files_names);
    ui->modelsList->setCurrentRow(0);
}

void MainWindow::updateProjection(QString filename)
{
    ui->teapotWg->updateProjection(filename);
}

void MainWindow::on_boxFlag_clicked()
{
    ui->teapotWg->boxControl(ui->boxFlag->isChecked());
}

void MainWindow::on_scale_flag_clicked()
{
    ui->teapotWg->scaleControl(ui->scale_flag->isChecked());
}
