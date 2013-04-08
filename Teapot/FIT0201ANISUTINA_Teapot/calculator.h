#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QVector3D>

class Calculator : public QThread
{
    Q_OBJECT
public:
    explicit Calculator(QObject *parent = 0);
    void run();
    void grabScene(QImage);
    void projectModel();
    void setStartPoint(QPoint);
    void setEndPoint(QPoint);
    void setSceneSize(QSize);
    void moveCameraPosition(double);
    void setPixelSafe(int, int, QRgb);
    void setPixelSafe(QPoint, QRgb);

    void drawLine(QPoint, QPoint, QRgb);
    void clearScene();
    void drawHLine(QPoint, QPoint);
    void drawVLine(QPoint, QPoint);
    QPoint project3Dto2Dscreen(QVector3D);
    void fillCoordinates();
    void drawTeapot();
    double bernstein(int dim, int indx, double arg);
    long factorial(int);
    
signals:
    void readyProjection(QImage);
    
public slots:

private:
   // QImage projection;
    QMutex mutex;

    QVector3D eye;
    QVector3D right;
    QVector3D up;
    QImage scene;
    QSize scene_size;
    qreal dist;
    QPoint start_point;
    QPoint end_point;
    float alpha;
    float beta;
    QVector<QVector3D> point_set;
    QVector<QVector3D> bezier_surface;
    QVector<QVector3D> test;
    QVector<QVector3D> bzr_crv_ctrl_pnts;
    int dimension1;
    int dimension2;
    int patch_number;
    
};

#endif // CALCULATOR_H
