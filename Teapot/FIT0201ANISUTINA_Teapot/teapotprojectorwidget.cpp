#include "teapotprojectorwidget.h"
#include "ui_teapotprojectorwidget.h"
#include <QPainter>

TeapotProjectorWidget::TeapotProjectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeapotProjectorWidget) {
    ui->setupUi(this);
    connect(&calculator, SIGNAL(readyProjection(QImage)), this, SLOT(redrawProjection(QImage)));
    calculator.projectModel();
}
TeapotProjectorWidget::~TeapotProjectorWidget() {
    delete ui;
}
void TeapotProjectorWidget::paintEvent(QPaintEvent *) {
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
    double numDegrees = e->delta() / 32.0;
    emit moveCamera(numDegrees);
    calculator.moveCameraPosition(numDegrees);
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
void TeapotProjectorWidget::changeDistanceToPlane(int dst) {
    calculator.setDistanceToPlane(dst);
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
void TeapotProjectorWidget::scaleControl(bool flag) {
    calculator.scaleControl(flag);
    calculator.projectModel();
}
void TeapotProjectorWidget::updateProjection(QString filename) {
    calculator.updateProjection(filename);
    calculator.projectModel();
}
