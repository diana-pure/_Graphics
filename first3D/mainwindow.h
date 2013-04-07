#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector3D>
#include <QMouseEvent>
#include <fstream>
#include <iostream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    QPoint translate(QVector3D x);
    void drawLine(QPoint, QPoint, QRgb);
    void clearScene();
    void drawHLine(QPoint, QPoint);
    void drawVLine(QPoint, QPoint);
    QPoint checkXYRange(QPoint);
    QPoint project3Dto2Dscreen(QVector3D);
    void fillCoordinates();
    void drawTeapot();
    double bernstein(int dim, int indx, double arg);
    long factorial(int);
    
private:
    Ui::MainWindow *ui;
    QVector3D eye;
    QVector3D right;
    QVector3D up;
    QImage scene;
    qreal dist;
    QPoint start_point;
    QPoint end_point;
    /*QVector3D apex1, apex2, apex3;
    QVector3D apex4, apex11, apex21;
    QVector3D apex31, apex41;*/
    float alpha;
    float beta;
    QVector<QVector3D> point_set;
    QVector<QVector3D> bezier_surface;
    QVector<QVector3D> test;
    int dimension1;
    int dimension2;
    int patch_number;
};

#endif // MAINWINDOW_H
