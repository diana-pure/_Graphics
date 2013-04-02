#include "curvewidget.h"
#include "ui_curvewidget.h"
#include <QPainter>
#include <QMouseEvent>

CurveWidget::CurveWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurveWidget)
{
    ui->setupUi(this);
    rubberBand = NULL;
    default_step = 25;
    default_scale = 1.0;
    STEP = default_step;
    scale = default_scale;
    curveThread.setStep(STEP);
    scene = QImage(500, 400, QImage::Format_RGB888);
    default_center = QPoint(scene.width() / 2, scene.height() / 2);
    center = default_center;
    clearScene();

    connect(&curveThread, SIGNAL(plotReady(QImage)), this, SLOT(redrawPlot(QImage)));
}
CurveWidget::~CurveWidget()
{
    delete ui;
    delete rubberBand;
}

void CurveWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, scene);
}
void CurveWidget::drawPlot()
{
    clearScene();
    curveThread.grabImage(scene);
    curveThread.graph();
}
void CurveWidget::redrawPlot(QImage plot)
{
    scene = plot;
    update();
}
void CurveWidget::clearScene()
{
    scene.fill(QColor(250, 250, 255));
    drawAxis();
}
void CurveWidget::drawAxis()
{
    drawHLine(QPoint(0, center.y()),QPoint(scene.width(), center.y()));
    drawVLine(QPoint(center.x(), 0), QPoint(center.x(), scene.height()));
    drawMarks();
}
void CurveWidget::drawMarks()
{
    for(int i = 0; i <= (scene.width() - center.x()) / STEP; i++) {
        drawVLine(QPoint(center.x() + i * STEP, center.y() - 3), QPoint(center.x() + i * STEP, center.y() + 3));
    }
    for(int i = 0; i <= center.x() / STEP; i++) {
        drawVLine(QPoint(center.x() - i * STEP, center.y() - 3), QPoint(center.x() - i * STEP, center.y() + 3));
    }
    for(int i = 0; i <= (scene.height() - center.y()) / STEP; i++) {
        drawHLine(QPoint(center.x() - 3, center.y() + i * STEP), QPoint(center.x() + 3, center.y() + i * STEP));
    }
    for(int i = 0; i <= center.y() / STEP; i++) {
        drawHLine(QPoint(center.x() - 3, center.y() - i * STEP), QPoint(center.x() + 3, center.y() - i * STEP));
    }
}
void CurveWidget::drawHLine(QPoint pnt1, QPoint pnt2)
{
    for(int i = pnt1.x(); i < pnt2.x(); i ++){
        scene.setPixel(checkXYRange(QPoint(i, pnt1.y())), QColor(0, 0, 0).rgba());
    }
}
void CurveWidget::drawVLine(QPoint pnt1, QPoint pnt2)
{
    for(int i = pnt1.y(); i < pnt2.y(); i ++){
        scene.setPixel(checkXYRange(QPoint(pnt1.x(), i)), QColor(0, 0, 0).rgba());
    }
}
QPoint CurveWidget::checkXYRange(QPoint pnt)
{
    int pnt_x = pnt.x(), pnt_y = pnt.y();
    if(0 > pnt.x()) { pnt_x = 0;   }
    if(scene.width() - 1 < pnt.x()) { pnt_x = (scene.width() - 1); }
    if(0 > pnt_y) { pnt_y = 0;   }
    if(scene.height() - 1 < pnt_y) { pnt_y = (scene.height() - 1); }
    return QPoint(pnt_x, pnt_y);
}
void CurveWidget::mousePressEvent(QMouseEvent *e)
{
    start_band = QPoint(qMin(e->pos().x(), scene.width()), qMin(e->pos().y(), scene.height()));
    if(!rubberBand) {
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    }
    rubberBand->setGeometry(QRect(start_band, QSize()));
    rubberBand->show();
}
void CurveWidget::mouseMoveEvent(QMouseEvent *e)
{
    rubberBand->setGeometry(QRect(start_band, e->pos()).normalized());
}
void CurveWidget::mouseReleaseEvent(QMouseEvent *e)
{
    rubberBand->hide();
    end_band = QPoint(qMax(0, qMin(e->pos().x(), scene.width())), qMax(0, qMin(e->pos().y(), scene.height())));
    scale *= scene.width() / static_cast<double>(qAbs(end_band.x() - start_band.x()));
    scale = (scale > 20) ? 20 : scale;
    STEP = default_step * scale;
    curveThread.setStep(STEP);
    center = QPoint(scale * (center.x() - qMin(start_band.x(), end_band.x())), scale * (center.y() - qMin(start_band.y(), end_band.y())));
    clearScene();
    curveThread.grabImage(scene, center);
    curveThread.graph();
    emit stepChanged(STEP);
    emit scaleChanged(scale);
}
void CurveWidget::setDefaults()
{
    STEP = default_step;
    scale = default_scale;
    center = default_center;
    curveThread.setStep(STEP);
    clearScene();
    curveThread.grabImage(scene, center);
    curveThread.graph();
    emit stepChanged(STEP);
    emit scaleChanged(scale);
}
