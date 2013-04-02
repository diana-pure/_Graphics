#include "curvethread.h"
#include "QMutexLocker"
#include <qmath.h>
#include <QColor>
#include <QDebug>

CurveThread::CurveThread(QObject *parent) :
    QThread(parent)
{
    default_step = 25;
    STEP = default_step;
}
CurveThread::~CurveThread(){}
void CurveThread::run()
{
    mutex.lock();
    plot = scene;
    double epsilon = 1e-6;
    double maxDx = 0.0, maxDy = 0.0, h =0.0;
    double MAGICCONST = 1 / (500.0 * (STEP / default_step));
    calculateTIntervals();
    for(int i = 0; i < 6; i++)
    {
        for(double t = t_store[2 * i]; t < t_store[2 * i + 1]; t +=h)
        {
            double eps = epsilon;
            double x = x_value(t);
            double y = y_value(t);
            do {
                eps *= 2;
                maxDx = qMax(derivativeX(t), derivativeX(t + eps));
                maxDy = qMax(derivativeY(t), derivativeY(t + eps));
                h = MAGICCONST / static_cast<double>((qSqrt(qPow(maxDx, 2) + qPow(maxDy, 2))));
            } while((h > eps) && (h < (t_store[2 * i + 1] - t_store[2 * i])));
            plot.setPixel(checkXYRange(convertCoordinateForPlot(x, y)), QColor(0, 0, 125).rgba());
        }
    }
    emit plotReady(plot);
    mutex.unlock();
}
void CurveThread::grabImage(QImage img)
{
    QMutexLocker locker(&mutex);
    scene = img;
    center = QPoint(scene.width() / 2, scene.height() / 2);
    Y_LOW = -(scene.height() / 2) / static_cast<double>(STEP);
    Y_HIGH = -Y_LOW;
    X_LEFT = -(scene.width() / 2) / static_cast<double>(STEP);
    X_RIGHT = -X_LEFT;
}
void CurveThread::grabImage(QImage img, QPoint cnt)
{
    QMutexLocker locker(&mutex);
    scene = img;
    center = cnt;
    Y_LOW = -((scene.height() - center.y()) / static_cast<double>(STEP));
    Y_HIGH = center.y() / static_cast<double>(STEP);
    X_LEFT = -(center.x() / static_cast<double>(STEP));
    X_RIGHT = (scene.width() - center.x()) / static_cast<double>(STEP);
}
void CurveThread::graph()
{
    QMutexLocker locker(&mutex);
    if(!isRunning()){
        start();
    }
}
double CurveThread::x_value(double t_param)
{
    return(qPow(t_param, 2) / (qPow(t_param, 2) - 1));
}
double CurveThread::y_value(double t_param)
{
    return((qPow(t_param, 2) + 1) / (t_param + 1));
}
double CurveThread::derivativeX(double tValue)
{
    return (-2 * tValue / static_cast<double>(qPow(qPow(tValue, 2), 2) - 1));
}
double CurveThread::derivativeY(double tValue)
{
    return ((2 * tValue - 1)/ static_cast<double>(qPow(tValue + 1, 2)));
}
QPoint CurveThread::checkXYRange(QPoint pnt)
{
    int pnt_x = pnt.x(), pnt_y = pnt.y();
    if(0 > pnt.x()) { pnt_x = 0;   }
    if(plot.width() - 1 < pnt.x()) { pnt_x = (plot.width() - 1); }
    if(0 > pnt_y) { pnt_y = 0;   }
    if(plot.height() - 1 < pnt_y) { pnt_y = (plot.height() - 1); }
    return QPoint(pnt_x, pnt_y);
}
QPoint CurveThread::convertCoordinateForPlot(double x_coord, double y_coord)
{
    return QPoint(center.x() + x_coord * STEP, center.y() - y_coord * STEP);
}
void CurveThread::setStep(int stp)
{
    QMutexLocker locker(&mutex);
    STEP = stp;
}
void CurveThread::calculateTIntervals()
{
    double T1 = -1 - qPow(2, 0.5);
    double T2 = 1 - qPow(2, 0.5);
    double T3 = 0.0;
    double y_low = Y_LOW;
    double y_high = Y_HIGH;
    double x_left = X_LEFT;
    double x_right = X_RIGHT;
    t_store[0] = (y_low - qPow((qPow(y_low, 2) - 4 * (1 - y_low)), 0.5)) / 2.0;
    t_store[1] = T1;
    t_store[2] = t_store[1];
    t_store[3] = (y_low + qPow((qPow(y_low, 2) - 4 * (1 - y_low)), 0.5)) / 2.0;
    t_store[4] = (y_high - qPow((qPow(y_high, 2) - 4 * (1 - y_high)), 0.5)) / 2.0;
    t_store[5] = T2;
    t_store[6] = t_store[5];
    t_store[7] = T3;
    t_store[8] = t_store[7];
    t_store[9] = qPow(-x_left / (1 - x_left), 0.5);
    t_store[10] = qPow(-x_right / (1 - x_right), 0.5);
    t_store[11] = (y_high + qPow((qPow(y_high, 2) - 4 * (1 - y_high)), 0.5)) / 2.0;
    for(int i = 0; i < 4; i++) {
        if (t_store[2 * i + 1] > t_store[2 * (i + 1)]) {
            t_store[2 * i + 1] = t_store[2 * (i + 1)];
        }
    }
    for(int i = 0; i < 4; i++) {
        if (t_store[2 * i] > t_store[2 * i + 1]) {
            t_store[2 * i] = t_store[2 * i + 1];
        }
    }
    t_store[3] = (t_store[3] > -1.001) ? -1.001 : t_store[3];
    t_store[4] = (t_store[4] < -0.999) ? -0.999 : t_store[4];
    t_store[9] = (t_store[9] > 0.99999) ? 0.99999 : t_store[9];
    t_store[10] = (t_store[10] < 1.00001) ? 1.00001 : t_store[10];
    t_store[11] = ((t_store[11] < 0.0) || (t_store[11] > 9.0)) ? 9.0 : t_store[11];
}
void CurveThread::setCenter(QPoint new_center)
{
    QMutexLocker locker(&mutex);
    center = new_center;
}
