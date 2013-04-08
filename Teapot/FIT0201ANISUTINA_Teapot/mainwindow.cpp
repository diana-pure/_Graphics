#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <teapotprojectorwidget.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    projection = QImage(200, 200, QImage::Format_RGB888);
    //calculator.projectModel();
    //connect(&calculator, SIGNAL(readyProjection(QImage)), this, SLOT(drawProjection(QImage)));
    //connect(ui->teapotWg, SIGNAL(readyProjection(QImage)), this, SLOT(drawProjection(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //QPainter painter(this);
    //painter.drawImage(0, 0, projection);
}

void MainWindow::drawProjection(QImage prj)
{
   // projection = prj;
    //update();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    //emit windowWasResized(size());
    //projection = QImage(e->size(), QImage::Format_RGB888);
    //projection.fill(QColor(255, 180, 180).rgba());
  /*  calculator.grabScene(projection);
    calculator.projectModel();*/

     ui->teapotWg->setSceneSize(e->size());
}
