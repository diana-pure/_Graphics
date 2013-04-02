#include "cassiniwidget.h"
#include "ui_cassiniwidget.h"
#include <QPainter>
#include <QDebug>

CassiniWidget::CassiniWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CassiniWidget)
{
    focus1 = QPoint(180,272);
    focus2 = QPoint(279, 107);
    STEP = 50;
    radius = 99;
    ui->setupUi(this);
    scene = QImage(500, 400, QImage::Format_RGB888);
    center = QPoint(scene.width() / 2, scene.height() / 2);
    cassiniThread.setExtraInfo(focus1, focus2, center, radius);
    clearScene();

    connect(&cassiniThread, SIGNAL(plotReady(QImage)), this, SLOT(redrawPlot(QImage)));
    emit wasSetFocus1(convertCoordinateForCanvas(focus1));
    emit wasSetFocus2(convertCoordinateForCanvas(focus2));
}
CassiniWidget::~CassiniWidget()
{
    delete ui;
}
void CassiniWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, scene);
}
void CassiniWidget::mousePressEvent(QMouseEvent *e)
{
    clearScene();
    focus1 = checkXYRange(e->pos());
    focus2 = focus1;
    drawFocus(focus1);
    update();
    emit wasSetFocus1(convertCoordinateForCanvas(focus1));
}
void CassiniWidget::mouseMoveEvent(QMouseEvent *e)
{
    focus2 = checkXYRange(e->pos());
    clearScene();
    drawFocus(focus1);
    drawFocus(focus2);
    drawLine(focus1, focus2);
    emit wasSetFocus2(convertCoordinateForCanvas(focus2));
    update();
}
void CassiniWidget::mouseReleaseEvent(QMouseEvent *e)
{
    focus2 = checkXYRange(e->pos());
    clearScene();
    drawFocus(focus1);
    drawFocus(focus2);
    update();
    emit wasSetFocus2(convertCoordinateForCanvas(focus2));
    cassiniThread.setExtraInfo(focus1, focus2, center, radius);
    cassiniThread.grabImage(scene);
    cassiniThread.graph();
}
void CassiniWidget::clearScene()
{
    scene.fill(QColor(250, 250, 255));
    drawAxis();
}
void CassiniWidget::drawAxis()
{
    drawHLine(QPoint(0, scene.height() / 2), QPoint(scene.width(), scene.height() / 2));
    drawVLine(QPoint(scene.width() / 2, 0),QPoint(scene.width() / 2, scene.height()));
    drawMarks();
}
void CassiniWidget::drawMarks()
{
    for(int i = 0; i <= scene.width() / (2 * STEP); i++) {
        drawVLine(QPoint(scene.width() / 2 + i * STEP, scene.height() / 2 - 3), QPoint(scene.width() / 2 + i * STEP, scene.height() / 2 + 3));
        drawVLine(QPoint(scene.width() / 2 - i * STEP, scene.height() / 2 - 3), QPoint(scene.width() / 2 - i * STEP, scene.height() / 2 + 3));
    }
    for(int i = 0; i <= scene.height() / (2 * STEP); i++) {
        drawHLine(QPoint(center.x() - 3, center.y() + i * STEP), QPoint(center.x() + 3, center.y() + i * STEP));
        drawHLine(QPoint(center.x() - 3, center.y() - i * STEP), QPoint(center.x() + 3, center.y() - i * STEP));
    }
}
void CassiniWidget::drawFocus(QPoint p)
{
    for(int i = p.x() - 2; i < p.x() + 2; i++) {
        for(int j = p.y() - 2; j < p.y() + 2; j++) {
            scene.setPixel(i, j, QColor(255, 0, 0).rgba());
        }
    }
}
void CassiniWidget::drawLine(QPoint pnt1, QPoint pnt2)
{
    int curr_x = pnt1.x(), curr_y = pnt1.y(), end_x = pnt2.x(), end_y = pnt2.y();
    if(curr_y == end_y) {
        drawHLine(pnt1, pnt2);
        return;
    }
    if(curr_x == end_x) {
        drawVLine(pnt1, pnt2);
        return;
    }
    int deltaX = qAbs(end_x - curr_x);
    int deltaY = qAbs(end_y - curr_y);
    int signX = curr_x < end_x ? 1 : -1;
    int signY = curr_y < end_y ? 1 : -1;
    int err = deltaX - deltaY;

    scene.setPixel(end_x, end_y, QColor(0, 0, 0).rgba());
    while((end_x != curr_x) || (end_y != curr_y))
    {
        scene.setPixel(curr_x, curr_y, QColor(0, 0, 0).rgba());
        int err2 = err * 2;
        if(err2 > -deltaY){
            err -= deltaY;
            curr_x += signX;
        }
        if(err2 < deltaX){
            err += deltaX;
            curr_y += signY;
        }
    }

}
void CassiniWidget::drawHLine(QPoint pnt1, QPoint pnt2)
{
    for(int i = pnt1.x(); i < pnt2.x(); i ++){
        scene.setPixel(checkXYRange(QPoint(i, pnt1.y())), QColor(0, 0, 0).rgba());
    }
}
void CassiniWidget::drawVLine(QPoint pnt1, QPoint pnt2)
{
    for(int i = pnt1.y(); i < pnt2.y(); i ++){
        scene.setPixel(checkXYRange(QPoint(pnt1.x(), i)), QColor(0, 0, 0).rgba());
    }
}
void CassiniWidget::drawPlot()
{
    drawFocus(focus1);
    drawFocus(focus2);
    cassiniThread.setExtraInfo(focus1, focus2, center, radius);
    cassiniThread.grabImage(scene);
    cassiniThread.graph();
}
void CassiniWidget::redrawPlot(QImage plot)
{
    scene = plot;
    update();
}
void CassiniWidget::drawPlot(QSize new_size)
{
    int new_width = new_size.width() < 100 ? 500 : new_size.width();
    int new_height = new_size.height() < 100 ? 400 : new_size.height();
    scene = QImage(new_width, new_height, QImage::Format_RGB888);
    center = QPoint(scene.width() / 2, scene.height() / 2);
    clearScene();
    drawFocus(focus1);
    drawFocus(focus2);
    cassiniThread.setExtraInfo(focus1, focus2, center, radius);
}
//extra 3 px for focus points
QPoint CassiniWidget::checkXYRange(QPoint pnt)
{
    int pnt_x = pnt.x(), pnt_y = pnt.y();
    if(3 > pnt.x()) { pnt_x = 3;   }
    if(scene.width() - 3 < pnt.x()) { pnt_x = (scene.width() - 3); }
    if(3 > pnt_y) { pnt_y = 3;   }
    if(scene.height() - 3 < pnt_y) { pnt_y = (scene.height() - 3); }
    return QPoint(pnt_x, pnt_y);
}
QPoint CassiniWidget::convertCoordinateForCanvas(QPoint pnt)
{
    return QPoint((pnt.x() - scene.width()/2), (scene.height()/2 - pnt.y()));
}
void CassiniWidget::setRadius(int rd)
{
    radius = rd;
    cassiniThread.setRadius(radius);
    cassiniThread.graph();
}
void CassiniWidget::setStep(int stp)
{
    STEP = stp;
}
