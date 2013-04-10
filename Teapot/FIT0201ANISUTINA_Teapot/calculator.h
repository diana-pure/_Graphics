#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QVector3D>
#include <QString>

class Calculator : public QThread
{
    Q_OBJECT
public:
    explicit Calculator(QObject *parent = 0);
    void fillCoordinates();
    void run();
    void clearScene();
    void drawAxis();
    void countAngles();
    void countVectors();
    double bernstein(int dim, int indx, double arg);
    int factorial(int);
    QPoint project3Dto2Dscreen(QVector3D);

    void projectModel();
    void setStartPoint(QPoint);
    void setEndPoint(QPoint);
    void setSceneSize(QSize);
    void setSegmentNum(int);
    void setCameraPosition(int);
    void moveCameraPosition(int);
    void axisControl(bool);

public slots:
    void updateProjection(QString);

    void drawLine(QPoint, QPoint, QRgb);
    void drawHLine(QPoint, QPoint, QRgb);
    void drawVLine(QPoint, QPoint, QRgb);
    void setPixelSafe(int, int, QRgb);
    void setPixelSafe(QPoint, QRgb);

signals:
    void readyProjection(QImage);

private:
    QImage scene;
    int NUM_SEGMENTS;
    QSize scene_size;
    QVector3D eye;
    QVector3D right;
    QVector3D up;
    QPoint start_point;
    QPoint end_point;
    float alpha;
    float beta;
    QVector<QVector3D> point_set;
    QVector<QVector3D> bzr_crv_ctrl_pnts;
    int patch_number;
    int dimension1;
    int dimension2;
    qreal dist;
    QRgb teapot_color;
    QMutex mutex;
    bool draw_axis_on;
    bool break_flag;
    QString filename;
    bool model_was_changed;
};

#endif // CALCULATOR_H
