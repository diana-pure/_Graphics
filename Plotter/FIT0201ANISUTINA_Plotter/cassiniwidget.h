#ifndef CASSINIWIDGET_H
#define CASSINIWIDGET_H

#include <cassinithread.h>
#include <QWidget>
#include<QMouseEvent>

namespace Ui {
class CassiniWidget;
}

class CassiniWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CassiniWidget(QWidget *parent = 0);
    ~CassiniWidget();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    QPoint convertCoordinateForCanvas(QPoint);
    QPoint checkXYRange(QPoint);
    void drawPlot();

protected:
    void drawFocus(QPoint);
    void drawLine(QPoint, QPoint);
    void drawHLine(QPoint, QPoint);
    void drawVLine(QPoint, QPoint);
    void clearScene();
    void drawAxis();
    void drawMarks();
    void setExtraInfo(QPoint fc1, QPoint fc2, QPoint cnt, int rd);

public slots:
    void redrawPlot(QImage);
    void setRadius(int rd);
    void drawPlot(QSize);
    void setStep(int);

signals:
    void wasSetFocus1(QPoint); //to notify ui thst focuses were pointed
    void wasSetFocus2(QPoint);
    
private:
    Ui::CassiniWidget *ui;
    QImage scene;
    QPoint focus1;
    QPoint focus2;
    QPoint center;
    int STEP;
    int radius;
    CassiniThread cassiniThread;
};

#endif // CASSINIWIDGET_H
