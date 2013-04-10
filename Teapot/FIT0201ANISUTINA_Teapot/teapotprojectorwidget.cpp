#include "teapotprojectorwidget.h"
#include "ui_teapotprojectorwidget.h"
#include <QPainter>

TeapotProjectorWidget::TeapotProjectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeapotProjectorWidget)
{
    ui->setupUi(this);
    connect(&calculator, SIGNAL(readyProjection(QImage)), this, SLOT(redrawProjection(QImage)));
    calculator.projectModel();
}

TeapotProjectorWidget::~TeapotProjectorWidget()
{
    delete ui;
}

void TeapotProjectorWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, scene);
}
void TeapotProjectorWidget::mousePressEvent(QMouseEvent *e) {
    calculator.setStartPoint(e->pos());
}
void TeapotProjectorWidget::mouseMoveEvent(QMouseEvent *e) {
    calculator.setEndPoint(e->pos());
    calculator.projectModel();
}
void TeapotProjectorWidget::wheelEvent(QWheelEvent *e) {
    int numDegrees = e->delta() / 320.0;
    calculator.moveCameraPosition(numDegrees);
    calculator.projectModel();
}

void TeapotProjectorWidget::drawProjection() {
    calculator.projectModel();
}
void TeapotProjectorWidget::redrawProjection(QImage proj) {
    scene = proj;
    update();
}

void TeapotProjectorWidget::setSceneSize(QSize sz) {
    calculator.setSceneSize(sz);
    calculator.projectModel();
}

void TeapotProjectorWidget::changeSegmentNumber(int num) {
    calculator.setSegmentNum(num);
    calculator.projectModel();
}

void TeapotProjectorWidget::changeDistance(int dst) {
    calculator.setCameraPosition(dst);
    calculator.projectModel();
}

void TeapotProjectorWidget::axisControl(bool flag) {
    calculator.axisControl(flag);
    calculator.projectModel();
}
void TeapotProjectorWidget::boxControl(bool flag) {
    calculator.boxControl(flag);
    calculator.projectModel();
}
void TeapotProjectorWidget::updateProjection(QString filename) {
    calculator.updateProjection(filename);
    calculator.projectModel();
}
