#ifndef CASSINITHREAD_H
#define CASSINITHREAD_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QVector>

class CassiniThread : public QThread
{
    Q_OBJECT
public:
    explicit CassiniThread(QObject *parent = 0);
    CassiniThread(QPoint fc1, QPoint fc2, QPoint cnt, int rd);
    ~CassiniThread();
    void setExtraInfo(QPoint fc1, QPoint fc2, QPoint cnt, int rd);
    void setRadius(int);
    //void setStep(int);
    void grabImage(QImage);
    void graph();
    
protected:
    void run();
    QPoint getStartPoint(QPoint, QPoint, int);
    long long cassiniFunc(QPoint, QPoint, QPoint, int);
    QPoint checkXYRange(QPoint);
    void swapFocuses();
    bool isOnLine(QPoint, QPoint, QPoint);

signals:
    void plotReady(QImage);
    
public slots:
    
private:
    QImage plot;
    QImage scene;
    QMutex mutex;
    QPoint focus1;
    QPoint focus2;
    QPoint center;
    QPoint startPoint;
    QVector<QPoint> envir;
    int radius;
};

#endif // CASSINITHREAD_H
