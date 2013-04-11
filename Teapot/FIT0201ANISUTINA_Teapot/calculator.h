#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QVector3D>
#include <QString>
#include <QWaitCondition>

class Calculator : public QThread
{
    Q_OBJECT
public:
    explicit Calculator(QObject *parent = 0);
    ~Calculator();
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
    void moveCameraPosition(double);
    void axisControl(bool);
    bool behindThePlane(QVector3D);
    void checkRangeForBox(QVector3D);
    void drawBox();
    void boxControl(bool);
    void scaleControl(bool);
    QImage drawSimpleTeapot();
    void setDistanceToPlane(int);
    void drawRib(QVector3D, QVector3D);
    QPoint lineCrossPlane(QVector3D, QVector3D);

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
    bool draw_box_on;
    bool break_flag;
    QString filename;
    bool model_was_changed;
    bool scale_was_changed;
    QVector3D center_projected;
    QVector<QVector3D> for_box;
    QWaitCondition condition;
    bool abort_flag;
    bool scale1000_on;
};

#endif // CALCULATOR_H
