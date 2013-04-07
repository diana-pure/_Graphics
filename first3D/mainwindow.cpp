#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMatrix4x4>
#include <qmath.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dist = 0.3;
    angle = 1.0;
    up = QVector3D(0, 0, 50);
    eye = QVector3D(300, 0, 0);
    scene = QImage(600, 400, QImage::Format_RGB888);
    scene.fill(QColor(255, 255, 255).rgba());
    //source = QFile("D:\_Учеба\Универ\_Gp\teapotCGA.bpt");

    fillCoordinates();

    test.append(QVector3D(0, 0, 0));
    test.append(QVector3D(0, 0, 50));
    test.append(QVector3D(0, 50, 60));
    test.append(QVector3D(0, 50, 10));
    test.append( QVector3D(-50, 0, 0));
    test.append( QVector3D(-50, 0, 50));
    test.append( QVector3D(-50, 50, 60));
    test.append( QVector3D(-50, 50, 10));
/*
    test.append(QVector3D(14, 0.0, 24));
    test.append(QVector3D(14, -07.84, 24));
    test.append(QVector3D(07.84, -14, 24));
    test.append(QVector3D(0.0, -14, 24));
    test.append(QVector3D(13.375, 0.0, 25.3125));
    test.append(QVector3D(13.375, -0.749, 25.3125));
    test.append(QVector3D(07.49, -13.375, 25.3125));
    test.append(QVector3D(0.0, -13.375, 25.3125));
*/
}


MainWindow::~MainWindow()
{
    delete ui;
    /*if(source) {
        source.close();
    }*/
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, scene);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    start_point = e->pos();
    angle = 1.0;
    scene.setPixel(e->pos().x(), e->pos().y(), QColor(255, 0, 0).rgba());
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    clearScene();

    scene.setPixel(e->pos().x(), e->pos().y(), QColor(255, 0, 0).rgba());
    end_point = e->pos();

    if((end_point.x() - start_point.x()) > 0) {
        angle += 1;
    } else {
        angle -= 1;
    }
    QMatrix4x4 matr_rot = QMatrix4x4();
    drawTeapot();

    for(int i = 0; i < test.size(); i++) {
        scene.setPixel(project3Dto2Dscreen(test.at(i)), QColor(255, 0, 0).rgba());
    }
/*
    for(int i = 0; i < bezier_surface.size() - 3; i += 4) {
        drawLine(checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i)))), checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i + 1)))), QColor(255, 0, 0).rgba());
        drawLine(checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i + 1)))), checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i + 2)))), QColor(255, 0, 0).rgba());
        drawLine(checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i + 2)))), checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i + 3)))), QColor(255, 0, 0).rgba());
        drawLine(checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i + 3)))), checkXYRange(translate(matr_rot.mapVector(bezier_surface.at(i + 1)))), QColor(255, 0, 0).rgba());
*/
       /* for(int j = i + 1; j < i + 16; j++) {
            drawLine(checkXYRange(translate(point_set.at(i))), checkXYRange(translate(point_set.at(j))), QColor(255, 0, 0).rgba());
        }*/
    //}

    update();
}


QPoint MainWindow::translate(QVector3D x){
    up.normalize();
    //eye.normalize();
    right = up.crossProduct(eye, up);
    QVector3D xpe = x + eye;
    QVector3D r0 =  (-1) * eye * dist;
    qreal d = up.dotProduct((r0 - x), eye) / up.dotProduct(xpe, eye);
    QVector3D a = x + d * xpe;
    QVector3D rightn = right.normalized();
    up.normalize();
    int Y = static_cast<int>(up.dotProduct(a, up) + .5);
    int X = static_cast<int>(up.dotProduct(a, rightn) + .5);
    return QPoint(X + scene.width() / 2, -Y + scene.height() / 2);
}

void MainWindow::drawLine(QPoint pnt1, QPoint pnt2, QRgb clr)
{
    int curr_x = pnt1.x(), curr_y = pnt1.y(), end_x = pnt2.x(), end_y = pnt2.y();
    if(curr_y == end_y) {
        drawHLine(pnt1, pnt2);
        return;
    }
    if(curr_x == end_x) {
        drawVLine(pnt1, pnt2);
        return;
    }
    int deltaX = qAbs(end_x - curr_x);
    int deltaY = qAbs(end_y - curr_y);
    int signX = curr_x < end_x ? 1 : -1;
    int signY = curr_y < end_y ? 1 : -1;
    int err = deltaX - deltaY;

    scene.setPixel(end_x, end_y, clr);
    while((end_x != curr_x) || (end_y != curr_y))
    {
        scene.setPixel(curr_x, curr_y, clr);
        int err2 = err * 2;
        if(err2 > -deltaY){
            err -= deltaY;
            curr_x += signX;
        }
        if(err2 < deltaX){
            err += deltaX;
            curr_y += signY;
        }
    }
}

void MainWindow::drawHLine(QPoint pnt1, QPoint pnt2)
{
    for(int i = pnt1.x(); i < pnt2.x(); i ++){
        scene.setPixel(checkXYRange(QPoint(i, pnt1.y())), QColor(0, 0, 0).rgba());
    }
}
void MainWindow::drawVLine(QPoint pnt1, QPoint pnt2)
{
    for(int i = pnt1.y(); i < pnt2.y(); i ++){
        scene.setPixel(checkXYRange(QPoint(pnt1.x(), i)), QColor(0, 0, 0).rgba());
    }
}

QPoint MainWindow::checkXYRange(QPoint pnt)
{
    int pnt_x = pnt.x(), pnt_y = pnt.y();
    if(0 > pnt.x()) {
        pnt_x = 0;
    }
    if(scene.width() - 1 < pnt.x()) {
        pnt_x = (scene.width() - 1);
    }
    if(0 > pnt_y) {
        pnt_y = 0;
    }
    if(scene.height() - 1 < pnt_y) {
        pnt_y = (scene.height() - 1);
    }
    return QPoint(pnt_x, pnt_y);
}

void MainWindow::clearScene(){
    scene.fill(QColor(255, 255, 255).rgba());
}

QPoint MainWindow::project3Dto2Dscreen(QVector3D point3D){
    QVector3D eye1 = eye.normalized();
    //eye.normalize();
    up.normalize();
    right = up.crossProduct(up, eye1);
    QVector3D center_projected = eye * dist;
    double t_star = - (eye.x() * (eye.x() - center_projected.x()) + eye.y() * (eye.y() - center_projected.y()) + eye.z() * (eye.z() - center_projected.z())) /
            (eye.x() * (point3D.x() - center_projected.x()) + eye.y() * (point3D.y() - center_projected.y()) + eye.z() * (point3D.z() - center_projected.z()) );
    QVector3D point3DonPlane3D = QVector3D(eye.x() + t_star * point3D.x(), eye.y() + t_star * point3D.y(), eye.z() + t_star * point3D.z());
    QPoint point3DinPlane2D = QPoint(right.dotProduct(right, point3DonPlane3D), up.dotProduct(up, point3DonPlane3D));
    return QPoint(scene.width() / 2 + point3DinPlane2D.x(), scene.height() / 2 - point3DinPlane2D.y());
}

void MainWindow::fillCoordinates() {
    std::ifstream source("D:\\TMP\\teapotCGA.bpt");
    if(!source) {
        std::cerr << "file was not open";
    }
         source >> patch_number;


         for(int i = 0; i < patch_number; i++) {
             source >> dimension1 >> dimension2;
             for(int j = 0; j < (dimension1 + 1) * (dimension2 + 1); j++) {
                 double x = 0.0, y = 0.0, z = 0.0;
                 source >> x >> y >> z;
                 point_set.append(QVector3D(x * 30, y * 30, z * 30));
             }
         }
}

void MainWindow::drawTeapot() {
    for(int ptch = 0; ptch < 3/*patch_number*/; ptch++)
    for(double u = 0.1; u < 1.0; u += 0.4) {
        for(double v = 0.1; v < 1.0; v += 0.4) {
            QVector3D surf_point(0, 0, 0);
            for(int i = 0; i < dimension1; i++) {
                for(int j = 0; j < dimension2; j++) {
                    surf_point += bernstein(dimension1, i, u) * bernstein(dimension2, j, v) * point_set.at(ptch * (dimension1 + 1) * (dimension2) + i * dimension1 + j);
                }
            }
            bezier_surface.append(surf_point);
            surf_point.setX(0.0);
            surf_point.setY(0.0);
            surf_point.setZ(0.0);
        }
    }
}

double MainWindow::bernstein(int dim, int indx, double arg) {
    long a = factorial(dim);
    qreal b = qPow(arg, indx);
    qreal c = qPow(1 - arg, dim - indx);
    double d = static_cast<double>(factorial(indx) * factorial(dim - indx));
    double result = a * b * c / d;
    return result;
}

long MainWindow::factorial(int arg) {
    long result = 1L;
    for(int i = arg; i > 1; i--) {
        result *= i;
    }

return result;
}
