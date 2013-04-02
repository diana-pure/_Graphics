#include "cassinithread.h"
#include <QMutexLocker>
#include <QColor>
#include <qmath.h>
#include <QDebug>

CassiniThread::CassiniThread(QObject *parent) :
    QThread(parent)
{
    radius = 0;
    envir = QVector<QPoint>();
}
CassiniThread::~CassiniThread(){}

void CassiniThread::run()
{
    mutex.lock();
    QPoint foc1 = focus1, foc2 = focus2;
    int rad = radius;
    plot = scene;
    mutex.unlock();
    for(int iter = 0; iter < 4; iter ++){
        startPoint = getStartPoint(foc1, foc2, rad);
        plot.setPixel(checkXYRange(startPoint), QColor(focus1.x() % 255, focus2.x() % 255, focus2.y() % 255).rgba());

        QPoint current = startPoint;
        QPoint previous = QPoint();

        for(int i = 0; i < 2000; i++)
        {
            int direct = ((iter % 2) == 0) ? 1 : -1;
            if(QPoint(current.x(), current.y() - (1 * direct)) != previous) {  envir.append(QPoint(current.x(), current.y() - (1 * direct))); }
            if(QPoint(current.x() + (1 * direct), current.y()) != previous) {  envir.append(QPoint(current.x() + (1 * direct), current.y())); }
            if(QPoint(current.x() - (1 * direct), current.y()) != previous) {  envir.append(QPoint(current.x() - (1 * direct), current.y())); }
            if(QPoint(current.x(), current.y() + (1 * direct)) != previous) {  envir.append(QPoint(current.x(), current.y() + (1 * direct))); }
            if(QPoint(current.x() - (1 * direct), current.y() - (1 * direct)) != previous) {  envir.append(QPoint(current.x() - (1 * direct), current.y() - (1 * direct))); }
            if(QPoint(current.x() - (1 * direct), current.y() + (1 * direct)) != previous) {  envir.append(QPoint(current.x() - (1 * direct), current.y() + (1 * direct))); }
            if(QPoint(current.x() + (1 * direct), current.y() + (1 * direct)) != previous) {  envir.append(QPoint(current.x() + (1 * direct), current.y() + (1 * direct))); }
            if(QPoint(current.x() + (1 * direct), current.y() - (1 * direct)) != previous) {  envir.append(QPoint(current.x() + (1 * direct), current.y() - (1 * direct))); }

            QVector<QPoint>::iterator iter = envir.begin();
            long long min = cassiniFunc(*iter, foc1, foc2, rad);

            int j = 0, index = 0;
            for(iter = envir.begin() + 1; iter != envir.end(); iter++)
            {
                j++;
                int value = cassiniFunc(*iter, foc1, foc2, rad);
                if(min > value) {
                    min = value;
                    index = j;
                }
            }
                plot.setPixel(checkXYRange(envir.at(index)), QColor(focus1.x() % 255, focus2.x() % 255, focus2.y() % 255).rgba());
                previous = current;
                current = envir.at(index);
            envir.clear();
        }
        //swapFocuses();
        qSwap(foc1, foc2);
    }
    emit plotReady(plot);
}
void CassiniThread::setRadius(int rd)
{
    QMutexLocker locker(&mutex);
    radius = rd;
}
void CassiniThread::setExtraInfo(QPoint fc1, QPoint fc2, QPoint cnt, int rd)
{
    QMutexLocker locker(&mutex);
    if(fc2.x() >= fc1.x()) {
        focus1 = fc1;
        focus2 = fc2;
    } else {
        focus1 = fc2;
        focus2 = fc1;
    }
    center = cnt;
    radius = rd;
}
void CassiniThread::grabImage(QImage img)
{
    QMutexLocker locker(&mutex);
    scene = img;

}
void CassiniThread::graph()
{
    QMutexLocker locker(&mutex);
    if(!isRunning()){
        start();
    }
}
long long CassiniThread::cassiniFunc(QPoint point, QPoint foc1, QPoint foc2, int rad)
{
  return qAbs((qPow(point.x() - foc1.x(), 2) + qPow(point.y() - foc1.y() , 2))
                * ((qPow(point.x() - foc2.x(), 2)) + qPow(point.y() - foc2.y(), 2)) - qPow(rad, 4));
}
QPoint CassiniThread::getStartPoint(QPoint foc1, QPoint foc2, int rad)
{
    int base_x = foc1.x(), base_y = foc1.y(), curr_x = foc2.x(), curr_y = foc2.y();
    int deltaX = qAbs(curr_x - base_x);
    int deltaY = qAbs(curr_y - base_y);
    if((base_x == curr_x) && (base_y == curr_y)) {  deltaX = 10; deltaY = 10;    }
    int signX = base_x < curr_x ? 1 : -1;
    int signY = base_y < curr_y ? 1 : -1;
    int err = deltaX - deltaY;

    long long prevDiffer = cassiniFunc(foc2, foc1, foc2, rad);
    long long currDiffer = prevDiffer;
    while(prevDiffer >= currDiffer)
    {
        prevDiffer = currDiffer;
        int err2 = err * 2;
        if(err2 > -deltaY){
            err -= deltaY;
            curr_x += signX;
        }
        if(err2 < deltaX){
            err += deltaX;
            curr_y += signY;
        }
        currDiffer = cassiniFunc(QPoint(curr_x, curr_y), foc1, foc2, rad);
    }
    return QPoint(curr_x, curr_y);
}
void CassiniThread::swapFocuses()
{
    QMutexLocker locker(&mutex);
    QPoint change = focus1;
    focus1 = focus2;
    focus2 = change;
}
QPoint CassiniThread::checkXYRange(QPoint pnt)
{
    int pnt_x = pnt.x(), pnt_y = pnt.y();
    if(0 > pnt.x()) { pnt_x = 0;   }
    if(plot.width() - 1 < pnt.x()) { pnt_x = (plot.width() - 1); }
    if(0 > pnt_y) { pnt_y = 0;   }
    if(plot.height() - 1 < pnt_y) { pnt_y = (plot.height() - 1); }
    return QPoint(pnt_x, pnt_y);
}
bool CassiniThread::isOnLine(QPoint pnt, QPoint foc1, QPoint foc2)
{
    int differ = ((pnt.x() - foc1.x()) * (foc2.y() - foc1.y()) - (pnt.y() - foc1.y()) * (foc2.x() - foc1.x()));
    return (qAbs(differ) < 2);
}
