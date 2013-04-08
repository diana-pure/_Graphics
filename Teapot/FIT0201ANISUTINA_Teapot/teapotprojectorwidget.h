#ifndef TEAPOTPROJECTORWIDGET_H
#define TEAPOTPROJECTORWIDGET_H

#include <QWidget>
#include <calculator.h>
#include <QMouseEvent>

namespace Ui {
class TeapotProjectorWidget;
}

class TeapotProjectorWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TeapotProjectorWidget(QWidget *parent = 0);
    ~TeapotProjectorWidget();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    //void mouseReleaseEvent(QMouseEvent *);
    void drawProjection();
    void setDistance(int);


public slots:
    void redrawProjection(QImage);
    
private:
    Ui::TeapotProjectorWidget *ui;
    Calculator calculator;
    QImage scene;
};

#endif // TEAPOTPROJECTORWIDGET_H
