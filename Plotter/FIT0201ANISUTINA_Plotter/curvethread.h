#ifndef CURVETHREAD_H
#define CURVETHREAD_H

#include <QThread>
#include <QMutex>
#include <QImage>

class CurveThread : public QThread
{
    Q_OBJECT
public:
    explicit CurveThread(QObject *parent = 0);
    ~CurveThread();
    void run();
    QPoint checkXYRange(QPoint p);
    void graph();
    void grabImage(QImage);
    void grabImage(QImage, QPoint cnt);
    void setStep(int);
    void setCenter(QPoint);
    void setBorders(double y_low, double y_high, double x_left, double x_right, int stp);
    double derivativeX(double);
    double derivativeY(double);
    QPoint convertCoordinateForCanvas(QPoint);
    QPoint convertCoordinateForPlot(double, double);
    double x_value(double t_param);
    double y_value(double t_param);

 protected:
    void calculateTIntervals();
    
signals:
    void plotReady(QImage);

public slots:
    
private:
    QImage plot;
    QImage scene;
    QMutex mutex;
    QPoint center;
    int STEP;
    int default_step;
    double t_store[12];
    double Y_LOW;
    double Y_HIGH;
    double X_LEFT;
    double X_RIGHT;
};

#endif // CURVETHREAD_H
