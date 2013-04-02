#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include "curvethread.h"
#include <QWidget>
#include <QPoint>
#include <QRubberBand>

namespace Ui {
class CurveWidget;
}

class CurveWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CurveWidget(QWidget *parent = 0);
    ~CurveWidget();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void clearScene();
    void clearScene(QPoint);
    void drawAxis();
    void drawMarks();
    void drawHLine(QPoint, QPoint);
    void drawVLine(QPoint, QPoint);
    QPoint checkXYRange(QPoint p);
    void drawPlot();
    void setDefaults();
    void recalcBorders();

public slots:
    void redrawPlot(QImage);

signals:
    void stepChanged(double);
    void scaleChanged(double);

private:
    Ui::CurveWidget *ui;
    QImage scene;
    int STEP;
    QPoint center;
    CurveThread curveThread;
    QPoint start_band;
    QPoint end_band;
    QRubberBand* rubberBand;
    double scale;
    int default_step;
    double default_scale;
    QPoint default_center;
};

#endif // CURVEWIDGET_H
