#include "calculator.h"
#include <QColor>
#include <QMutexLocker>
#include <qmath.h>
#include <QMatrix4x4>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <QFile>

Calculator::Calculator(QObject *parent) :
    QThread(parent)
{
    scene = QImage(scene_size, QImage::Format_RGB888);
    NUM_SEGMENTS = 1;
    scene_size = QSize(600, 400);
    eye = QVector3D(300, 0, 0);
    up = QVector3D(0, 0, 50);
    alpha = 1.0;
    beta = 1.0;
    dist = 150;
    teapot_color = QColor(222, 49, 99).rgba();
    draw_axis_on = true;//false;
    break_flag = false;
    filename = QString("teapot.bpt");
    fillCoordinates();   // was every filed initialized
}
void Calculator::fillCoordinates() {
    QString name_to_load(":/");
    name_to_load.append(filename);
    QFile file(name_to_load);
    if(!file.open(QIODevice::ReadOnly)) {
        file.close();
    }
    QTextStream source(&file);
    point_set.clear();
    source >> patch_number;
    for(int i = 0; i < patch_number; i++) {
        source >> dimension1 >> dimension2;
        for(int j = 0; j < (dimension1 + 1) * (dimension2 + 1); j++) {
            double x = 0.0, y = 0.0, z = 0.0;
            source >> x >> y >> z;
            point_set.append(QVector3D(x * 30, y * 30, z * 30));
            //point_set.append(QVector3D(x, y, z));
        }
    }
    model_was_changed = false;
    file.close();
}
void Calculator::run()
{
    if(model_was_changed) {
        fillCoordinates();
    }
    mutex.lock();
        int NUM_SEGMENTS = this->NUM_SEGMENTS;
        QSize scene_size = this->scene_size;
      //  QVector3D eye = this->eye;
        QPoint start_point = this->start_point;
        QPoint end_point = this->end_point;
        bool draw_axis_on = this->draw_axis_on;
    mutex.unlock();

    //clearScene(); //is it necessary to protect when size is chanched
    //countAngles();
    //countVectors();

    scene = QImage(scene_size, QImage::Format_RGB888);
    scene.fill(QColor(255, 255, 255).rgba());

    int delta_x = start_point.x() - end_point.x();
    int delta_y = start_point.y() - end_point.y();
    int big_distance2 = qPow(eye.length(), 2);
    float alpha = 10 * qAsin(delta_x * qPow(big_distance2 - qPow(delta_x, 2) / 4, 0.5) / big_distance2);
    float beta = 10 * qAsin(delta_y * qPow(big_distance2 - qPow(delta_y, 2) / 4, 0.5) / big_distance2);

    mutex.lock();
        this->end_point = this->start_point;
    mutex.unlock();

    QMatrix4x4 matr_rot = QMatrix4x4();
    matr_rot.rotate(alpha, up);
    eye = matr_rot.mapVector(eye);
    right = up.crossProduct(up, eye);
    matr_rot.rotate(beta, right);
    eye = matr_rot.mapVector(eye);
    up = matr_rot.mapVector(up);

    if(draw_axis_on) {
        drawAxis();
    }

    double STEP_discr = 1.0 / static_cast<double>(NUM_SEGMENTS);
    double STEP_contin = 0.001;
    for(int ptch = 0; ptch < patch_number; ptch++) {
        //vertical lines
        for(double u = 0.0; u <= 1.0; u += STEP_discr) {
            // get control points for bezier curve
            //draw bezier curve
            bzr_crv_ctrl_pnts.clear();
            for(int j = 0; j <= dimension2; j++) {
                QVector3D curve_point(0, 0, 0);
                for(int i = 0; i <= dimension1; i++) {
                    curve_point += bernstein(dimension1, i, u) * point_set.at(ptch * (dimension1 + 1) * (dimension2 + 1) + j * (dimension1 + 1) + i);
                }
                bzr_crv_ctrl_pnts.append(curve_point);
                curve_point = QVector3D(0, 0, 0);
            }
            for(double v = 0.0; v <= 1.0; v += STEP_contin) {
                QVector3D surf_point(0, 0, 0);
                    for(int j = 0; j <= dimension2; j++) {
                        surf_point += bernstein(dimension2, j, v) * bzr_crv_ctrl_pnts.at(j);
                    }
                    setPixelSafe(project3Dto2Dscreen(surf_point), teapot_color);
                    surf_point = QVector3D(0, 0, 0);
            }
        }
        //horisontal lines
        for(double v = 0.0; v <= 1.0; v += STEP_discr) {
            // get control points for bezier curve
            //draw bezier curve
            bzr_crv_ctrl_pnts.clear();
            for(int i = 0; i <= dimension1; i++) {
                QVector3D curve_point(0, 0, 0);
                for(int j = 0; j <= dimension2; j++) {
                    curve_point += bernstein(dimension2, j, v) * point_set.at(ptch * (dimension1 + 1) * (dimension2 + 1) + j * (dimension1 + 1) + i);
                }
                bzr_crv_ctrl_pnts.append(curve_point);
                curve_point = QVector3D(0, 0, 0);
            }
            for(double u = 0.0; u <= 1.0; u += STEP_contin) {
                QVector3D surf_point(0, 0, 0);
                    for(int i = 0; i <= dimension1; i++) {
                        surf_point += bernstein(dimension1, i, u) * bzr_crv_ctrl_pnts.at(i);
                    }
                    setPixelSafe(project3Dto2Dscreen(surf_point), teapot_color);
                    surf_point = QVector3D(0, 0, 0);
            }
        }
    /*    if(true == break_flag) {
            mutex.lock();
                break_flag = false;
            mutex.unlock();
            return;
        }*/
    }
    emit readyProjection(scene);
}
void Calculator::clearScene() {
    scene = QImage(scene_size, QImage::Format_RGB888);
    scene.fill(QColor(255, 255, 255).rgba());
}
void Calculator::drawAxis() {
    QVector3D center(0, 0, 0);
    QVector3D x_axis(50, 0, 0);
    QVector3D y_axis(0, 50, 0);
    QVector3D z_axis(0, 0, 50);

    drawLine(project3Dto2Dscreen(center), project3Dto2Dscreen(x_axis), QColor(255, 0, 0).rgba());
    drawLine(project3Dto2Dscreen(center), project3Dto2Dscreen(y_axis), QColor(0, 255, 0).rgba());
    drawLine(project3Dto2Dscreen(center), project3Dto2Dscreen(z_axis), QColor(0, 0, 255).rgba());
}
void Calculator::countAngles() {
    mutex.lock();
        QPoint start_point = this->start_point;
        QPoint end_point = this->end_point;
    mutex.unlock();
    int delta_x = start_point.x() - end_point.x();
    int delta_y = start_point.y() - end_point.y();
    int big_distance2 = qPow(eye.length(), 2);
    alpha = 10 * qAsin(delta_x * qPow(big_distance2 - qPow(delta_x, 2) / 4, 0.5) / big_distance2);
    beta = 10 * qAsin(delta_y * qPow(big_distance2 - qPow(delta_y, 2) / 4, 0.5) / big_distance2);
    mutex.lock();
        this->end_point = this->start_point;
    mutex.unlock();

}
void Calculator::countVectors() {
    mutex.lock();
        float alpha = this->alpha;
        float beta = this->beta;
    mutex.unlock();
    QMatrix4x4 matr_rot = QMatrix4x4();
    matr_rot.rotate(alpha, up);
    eye = matr_rot.mapVector(eye);
    right = up.crossProduct(up, eye);
    matr_rot.rotate(beta, right);
    eye = matr_rot.mapVector(eye);
    up = matr_rot.mapVector(up);

}
double Calculator::bernstein(int dim, int indx, double arg) {
    return factorial(dim) * qPow(arg, indx) * qPow(1 - arg, dim - indx) / static_cast<double>(factorial(indx) * factorial(dim - indx));;
}
int Calculator::factorial(int arg) {
    int result = 1;
    for(int i = arg; i > 1; i--) {
        result *= i;
    }
return result;
}
QPoint Calculator::project3Dto2Dscreen(QVector3D point3D) { //todo: make easier to understand
    QVector3D eye1 = eye.normalized();
    up.normalize();
    right = up.crossProduct(up, eye1);
    QVector3D center_projected = eye - (eye.length() - dist) * eye.normalized();
    double t_star = - (eye.x() * (eye.x() - center_projected.x()) + eye.y() * (eye.y() - center_projected.y()) + eye.z() * (eye.z() - center_projected.z())) /
            (eye.x() * (point3D.x() - center_projected.x()) + eye.y() * (point3D.y() - center_projected.y()) + eye.z() * (point3D.z() - center_projected.z()) );
    QVector3D point3DonPlane3D = QVector3D(eye.x() + t_star * point3D.x(), eye.y() + t_star * point3D.y(), eye.z() + t_star * point3D.z());
    QPoint point3DinPlane2D = QPoint(right.dotProduct(right, point3DonPlane3D), up.dotProduct(up, point3DonPlane3D));
    return QPoint(scene.width() / 2 + point3DinPlane2D.x(), scene.height() / 2 - point3DinPlane2D.y());
}
void Calculator::projectModel()
{
    QMutexLocker locker(&mutex);
    if(!isRunning()){
        start();
    }
}
void Calculator::setStartPoint(QPoint pnt) {
    QMutexLocker locker(&mutex);
    start_point = pnt;
    break_flag = true;
}
void Calculator::setEndPoint(QPoint pnt) {
    QMutexLocker locker(&mutex);
    end_point = pnt;
    break_flag = true;
}
void Calculator::setSceneSize(QSize sz) {
    QMutexLocker locker(&mutex);
    scene_size = sz;
    break_flag = true;
}
void Calculator::setSegmentNum(int num) {
    QMutexLocker locker(&mutex);
    NUM_SEGMENTS = (0 == num) ? 1 : num;
    break_flag = true;
}
void Calculator::setCameraPosition(int dst) {
    QMutexLocker locker(&mutex);
    eye = dst * eye.normalized();
    break_flag = true;
}
void Calculator::moveCameraPosition(int dst) {
    QMutexLocker locker(&mutex);
    eye = (eye.length() + dst) * eye.normalized();
    break_flag = true;
}
void Calculator::axisControl(bool flag) {
    QMutexLocker locker(&mutex);
    draw_axis_on = flag;
    break_flag = true;
}

void Calculator::updateProjection(QString name) {
    QMutexLocker locker(&mutex);
    filename = name;
    model_was_changed = true;
    break_flag = true;
}

void Calculator::drawLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
    int curr_x = pnt1.x(), curr_y = pnt1.y(), end_x = pnt2.x(), end_y = pnt2.y();
    if(curr_y == end_y) {
        drawHLine(pnt1, pnt2, clr);
        return;
    }
    if(curr_x == end_x) {
        drawVLine(pnt1, pnt2, clr);
        return;
    }
    int deltaX = qAbs(end_x - curr_x);
    int deltaY = qAbs(end_y - curr_y);
    int signX = curr_x < end_x ? 1 : -1;
    int signY = curr_y < end_y ? 1 : -1;
    int err = deltaX - deltaY;

    setPixelSafe(end_x, end_y, clr);
    while((end_x != curr_x) || (end_y != curr_y))
    {
        setPixelSafe(curr_x, curr_y, clr);
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
void Calculator::drawHLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
    for(int i = pnt1.x(); i < pnt2.x(); i ++){
        setPixelSafe(QPoint(i, pnt1.y()), clr);
    }
}
void Calculator::drawVLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
    for(int i = pnt1.y(); i < pnt2.y(); i ++){
        setPixelSafe(QPoint(pnt1.x(), i), clr);
    }
}
void Calculator::setPixelSafe(int x_coord, int y_coord, QRgb color) {
    if((0 > x_coord) || (scene.width() - 1 < x_coord))
    {
        return;
    }
    if((0 > y_coord) || (scene.height() - 1 < y_coord)){
        return;
    }
    scene.setPixel(x_coord, y_coord, color);
    return;
}
void Calculator::setPixelSafe(QPoint pnt, QRgb color) {
    setPixelSafe(pnt.x(), pnt.y(), color);
}
