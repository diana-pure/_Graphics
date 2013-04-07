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
    dist = 0.1;
    angle = 1.0;
    up = QVector3D(0, 0, 50);
    eye = QVector3D(100, 0, 0);
    scene = QImage(800, 600, QImage::Format_RGB888);
    scene.fill(QColor(255, 255, 255).rgba());
    //source = QFile("D:\_Учеба\Универ\_Gp\teapotCGA.bpt");

    fillCoordinates();
/*
    apex1 = QVector3D(10, 10, 10);
    apex2 = QVector3D(10, 10, 50);
    apex3 = QVector3D(10, 50, 60);
    apex4 = QVector3D(10, 50, 20);
    apex11 = QVector3D(50, 10, 10);
    apex21 = QVector3D(50, 10, 50);
    apex31 = QVector3D(50, 50, 60);
    apex41 = QVector3D(50, 50, 20);*/

    apex1 = QVector3D(14, 0.0, 24);
    apex2 = QVector3D(14, -07.84, 24);
    apex3 = QVector3D(07.84, -14, 24);
    apex4 = QVector3D(0.0, -14, 24);
    apex11 = QVector3D(13.375, 0.0, 25.3125);
    apex21 = QVector3D(13.375, -0.749, 25.3125);
    apex31 = QVector3D(07.49, -13.375, 25.3125);
    apex41 = QVector3D(0.0, -13.375, 25.3125);
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
    matr_rot.rotate(angle, apex1);
    apex1 = matr_rot.mapVector(apex1);
    apex2 = matr_rot.mapVector(apex2);
    apex3 = matr_rot.mapVector(apex3);
    apex4 = matr_rot.mapVector(apex4);
    apex11 = matr_rot.mapVector(apex11);
    apex21 = matr_rot.mapVector(apex21);
    apex31 = matr_rot.mapVector(apex31);
    apex41 = matr_rot.mapVector(apex41);
    QPoint pnt1 = checkXYRange(translate(apex1));
    QPoint pnt2 = checkXYRange(translate(apex2));
    QPoint pnt3 = checkXYRange(translate(apex3));
    QPoint pnt4 = checkXYRange(translate(apex4));
    QPoint pnt11 = checkXYRange(translate(apex11));
    QPoint pnt21 = checkXYRange(translate(apex21));
    QPoint pnt31 = checkXYRange(translate(apex31));
    QPoint pnt41 = checkXYRange(translate(apex41));

    drawLine(pnt2, pnt1, QColor(255, 0, 0).rgba());
    drawLine(pnt2, pnt3, QColor(255, 0, 0).rgba());
    drawLine(pnt3, pnt4, QColor(255, 0, 0).rgba());
    drawLine(pnt1, pnt4, QColor(255, 0, 0).rgba());
    drawLine(pnt21, pnt11, QColor(0, 0, 255).rgba());
    drawLine(pnt21, pnt31, QColor(0, 0, 255).rgba());
    drawLine(pnt31, pnt41, QColor(0, 0, 255).rgba());
    drawLine(pnt11, pnt41, QColor(0, 0, 255).rgba());
    drawLine(pnt21, pnt2, QColor(0, 0, 255).rgba());
    drawLine(pnt31, pnt3, QColor(0, 0, 255).rgba());
    drawLine(pnt41, pnt4, QColor(0, 0, 255).rgba());
    drawLine(pnt11, pnt1, QColor(0, 0, 255).rgba());

   /* for(int i = 0; i < point_set.size(); i++) {
        scene.setPixel(checkXYRange(translate(point_set.at(i))), QColor(255, 0, 0).rgba());
    }*/
/*
    for(int i = 0; i < 1/*point_set.size()* /; i += 16) {
        drawLine(checkXYRange(translate(point_set.at(i))), checkXYRange(translate(point_set.at(i + 1))), QColor(255, 0, 0).rgba());
        drawLine(checkXYRange(translate(point_set.at(i + 1))), checkXYRange(translate(point_set.at(i + 2))), QColor(255, 0, 0).rgba());
        drawLine(checkXYRange(translate(point_set.at(i + 2))), checkXYRange(translate(point_set.at(i + 3))), QColor(255, 0, 0).rgba());
        drawLine(checkXYRange(translate(point_set.at(i + 3))), checkXYRange(translate(point_set.at(i + 1))), QColor(255, 0, 0).rgba());

        for(int j = i + 1; j < i + 16; j++) {
            drawLine(checkXYRange(translate(point_set.at(i))), checkXYRange(translate(point_set.at(j))), QColor(255, 0, 0).rgba());
        }
    }
*/
    update();
    //}
}


QPoint MainWindow::translate(QVector3D x){
    up.normalize();
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
    eye.normalize();
    up.normalize();
    right = up.crossProduct(up, eye);
    QVector3D center_projected = eye * dist;
    double t_star = - (eye.x() * center_projected.x() + eye.y() * center_projected.y() + eye.y() * center_projected.z()) /
            (eye.x() * (point3D.x() - center_projected.x()) + eye.y() * (point3D.y() - center_projected.y()) + eye.z() * (point3D.z() - center_projected.z()) );
    QVector3D point3DonPlane3D = t_star * point3D;
    QPoint point3DinPlane2D = QPoint(right.dotProduct(right, point3DonPlane3D), up.dotProduct(up, point3DonPlane3D));
    return QPoint(point3DinPlane2D.x() + scene.width() / 2, point3DinPlane2D.y() - scene.height() / 2);
}

void MainWindow::fillCoordinates() {
    std::ifstream source("D:\\TMP\\teapotCGA.bpt");
    if(!source) {
        std::cerr << "file was not open";
    }
    /*
    if (!source.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
*/
         int patch_number = 0;
         source >> patch_number;


         for(int i = 0; i < patch_number; i++) {
             int dimension1 = 0, dimension2 = 0;
             source >> dimension1 >> dimension2;
             for(int j = 0; j < (dimension1 + 1) * (dimension2 + 1); j++) {
                 double x = 0.0, y = 0.0, z = 0.0;
                 source >> x >> y >> z;
                 point_set.append(QVector3D(x * 30, y * 30, z * 30));
             }
         }
}
