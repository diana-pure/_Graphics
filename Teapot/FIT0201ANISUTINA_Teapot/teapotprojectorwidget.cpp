#include "teapotprojectorwidget.h"
#include "ui_teapotprojectorwidget.h"
#include <QPainter>

TeapotProjectorWidget::TeapotProjectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeapotProjectorWidget)
{
    ui->setupUi(this);
    scene = QImage(600, 400, QImage::Format_RGB888);
    calculator.grabScene(scene);
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
void TeapotProjectorWidget::drawProjection() {
    calculator.grabScene(scene);
    calculator.projectModel();
}
void TeapotProjectorWidget::redrawProjection(QImage proj) {
    scene = proj;
    update();
}
