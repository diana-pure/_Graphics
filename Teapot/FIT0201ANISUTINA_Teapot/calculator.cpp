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
    QThread(parent) {
    scene = QImage(scene_size, QImage::Format_RGB888);
    NUM_SEGMENTS = 1;
    scene_size = QSize(600, 400);
    eye = QVector3D(153, 0, 0);
    up = QVector3D(0, 0, 1);
    alpha = 1.0;
    beta = 1.0;
    dist = 150;
    teapot_color = QColor(222, 49, 99).rgba();
    draw_axis_on = true;
    draw_box_on = false;
    break_flag = false;
    filename = QString("teapot.bpt");
    fillCoordinates();   // was every filed initialized
    center_projected = (eye.length() - dist) * eye.normalized();
    abort_flag = false;
    scale1000_on = false;
}
Calculator::~Calculator() {
    mutex.lock();
        abort_flag = true;
        condition.wakeOne();
    mutex.unlock();
    wait();
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
            if(scale1000_on) {
                point_set.append(QVector3D(x * 10, y * 10, z * 10));
            } else {
                point_set.append(QVector3D(x, y, z));
            }
        }
    }
    model_was_changed = false;
    scale_was_changed = false;
    file.close();
    for_box.clear();
    QVector3D pnt(0.0, 0.0, 0.0);
    for(int i =0; i < 6; i++) {
        for_box.append(pnt);
    }
}
void Calculator::run() {
    while(true) {
        if(model_was_changed || scale_was_changed) {
            fillCoordinates();
        }

        mutex.lock();
            int NUM_SEGMENTS = this->NUM_SEGMENTS;
            QSize scene_size = this->scene_size;
          //  QVector3D eye = this->eye;
            QPoint start_point = this->start_point;
            QPoint end_point = this->end_point;
            bool draw_axis_on = this->draw_axis_on;
            //bool draw_box_on = this->draw_box_on;
        mutex.unlock();

        //clearScene(); //is it necessary to protect when size is chanched
        //countAngles();
        //countVectors();
        if(0 == NUM_SEGMENTS) {
            scene = drawSimpleTeapot();
        } else {
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
        double STEP_contin = 0.0006;
        for(int ptch = 0; ptch < patch_number; ptch++) {

            if(break_flag) {
                break;
            }
            if(abort_flag) {
                return;
            }
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
                        if(behindThePlane(surf_point)) {
                            setPixelSafe(project3Dto2Dscreen(surf_point), teapot_color);
                        }
                        checkRangeForBox(surf_point);
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
                        if(behindThePlane(surf_point)) {
                            setPixelSafe(project3Dto2Dscreen(surf_point), teapot_color);
                        }
                        checkRangeForBox(surf_point);
                        surf_point = QVector3D(0, 0, 0);
                }
            }
        }
        }
        if(draw_box_on) {
            drawBox();
        }

        if (!break_flag) {
            emit readyProjection(scene);
        }
        mutex.lock();
            if (!break_flag) {
                condition.wait(&mutex);
            }
            break_flag = false;
        mutex.unlock();
    }
}
void Calculator::clearScene() {
    scene = QImage(scene_size, QImage::Format_RGB888);
    scene.fill(QColor(255, 255, 255).rgba());
}
void Calculator::drawAxis() {
    QVector3D center(0, 0, 0);
    QVector3D x_axis(10, 0, 0);
    QVector3D y_axis(0, 10, 0);
    QVector3D z_axis(0, 0, 10);

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
    right = up.crossProduct(up, eye.normalized());
    center_projected = (eye.length() - dist) * eye.normalized();
    double t_star = - (eye.x() * (eye.x() - center_projected.x()) + eye.y() * (eye.y() - center_projected.y()) + eye.z() * (eye.z() - center_projected.z())) /
            (eye.x() * (point3D.x() - center_projected.x()) + eye.y() * (point3D.y() - center_projected.y()) + eye.z() * (point3D.z() - center_projected.z()) );
    QVector3D point3DonPlane3D = QVector3D(eye.x() + t_star * point3D.x(), eye.y() + t_star * point3D.y(), eye.z() + t_star * point3D.z());
    QPoint point3DinPlane2D = QPoint(right.dotProduct(right, point3DonPlane3D), up.dotProduct(up, point3DonPlane3D));
    return QPoint(scene.width() / 2 + point3DinPlane2D.x(), scene.height() / 2 - point3DinPlane2D.y());
}
bool Calculator::behindThePlane(QVector3D point3D) {
    return ((eye.x() * (point3D.x() - center_projected.x()) + eye.y() * (point3D.y() - center_projected.y()) + eye.z() * (point3D.z() - center_projected.z())) *
            (eye.x() * (0.0 - center_projected.x()) + eye.y() * (0.0 - center_projected.y()) + eye.z() * (0.0 - center_projected.z())) > 0);
}
void Calculator::checkRangeForBox(QVector3D pnt) {
    if(pnt.x() < for_box.at(0).x()) {
        for_box.replace(0, pnt);
    }
    if(pnt.x() > for_box.at(1).x()) {
        for_box.replace(1, pnt);
    }
    if(pnt.y() < for_box.at(2).y()) {
        for_box.replace(2, pnt);
    }
    if(pnt.y() > for_box.at(3).y()) {
        for_box.replace(3, pnt);
    }
    if(pnt.z() < for_box.at(4).z()) {
        for_box.replace(4, pnt);
    }
    if(pnt.z() > for_box.at(5).z()) {
        for_box.replace(5, pnt);
    }
}
void Calculator::drawBox() {
    mutex.lock();
    QVector<QVector3D> for_box = this->for_box;
    mutex.unlock();
    QVector3D begin_line(for_box.at(0).x(), for_box.at(2).y(), for_box.at(4).z());
    QVector3D end_line(for_box.at(0).x(), for_box.at(3).y(), for_box.at(4).z());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setY(for_box.at(3).y());
    end_line.setX(for_box.at(1).x());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setX(for_box.at(1).x());
    end_line.setY(for_box.at(2).y());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setY(for_box.at(2).y());
    end_line.setX(for_box.at(0).x());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);


    begin_line = QVector3D(for_box.at(0).x(), for_box.at(2).y(), for_box.at(5).z());
    end_line = QVector3D(for_box.at(0).x(), for_box.at(3).y(), for_box.at(5).z());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setY(for_box.at(3).y());
    end_line.setX(for_box.at(1).x());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setX(for_box.at(1).x());
    end_line.setY(for_box.at(2).y());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setY(for_box.at(2).y());
    end_line.setX(for_box.at(0).x());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);


    begin_line = QVector3D(for_box.at(0).x(), for_box.at(2).y(), for_box.at(4).z());
    end_line = QVector3D(for_box.at(0).x(), for_box.at(2).y(), for_box.at(5).z());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setY(for_box.at(3).y());
    end_line.setY(for_box.at(3).y());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setX(for_box.at(1).x());
    end_line.setX(for_box.at(1).x());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);
    begin_line.setY(for_box.at(2).y());
    end_line.setY(for_box.at(2).y());
    drawLine(project3Dto2Dscreen(begin_line), project3Dto2Dscreen(end_line), teapot_color);

}
void Calculator::projectModel()
{
    QMutexLocker locker(&mutex);
    if(!isRunning()){
        start();
    } else {
        break_flag = true;
        condition.wakeOne();
    }
}
void Calculator::setStartPoint(QPoint pnt) {
    QMutexLocker locker(&mutex);
    start_point = pnt;
}
void Calculator::setEndPoint(QPoint pnt) {
    QMutexLocker locker(&mutex);
    end_point = pnt;
}
void Calculator::setSceneSize(QSize sz) {
    QMutexLocker locker(&mutex);
    scene_size = sz;
}
void Calculator::setSegmentNum(int num) {
    QMutexLocker locker(&mutex);
    //NUM_SEGMENTS = (0 == num) ? 1 : num;
    NUM_SEGMENTS = num;
}
void Calculator::setCameraPosition(int dst) {
    QMutexLocker locker(&mutex);
    if(dst > dist) {
        eye = dst * eye.normalized();
    }
}
void Calculator::setDistanceToPlane(int dst) {
    QMutexLocker locker(&mutex);
    if(dst >= 20) {
        dist = dst;
    }
}
void Calculator::moveCameraPosition(double dst) {
    QMutexLocker locker(&mutex);
    if(eye.length() - dst > dist) {
        eye = (eye.length() - dst) * eye.normalized();
    }
}
void Calculator::axisControl(bool flag) {
    QMutexLocker locker(&mutex);
    draw_axis_on = flag;
}
void Calculator::boxControl(bool flag) {
    QMutexLocker locker(&mutex);
    draw_box_on = flag;
}
void Calculator::scaleControl(bool flag) {
    QMutexLocker locker(&mutex);
    scale1000_on = flag;
    scale_was_changed = true;
}
void Calculator::updateProjection(QString name) {
    QMutexLocker locker(&mutex);
    filename = name;
    model_was_changed = true;
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
        if(err2 > -deltaY) {
            err -= deltaY;
            curr_x += signX;
        }
        if(err2 < deltaX) {
            err += deltaX;
            curr_y += signY;
        }
    }
}
void Calculator::drawHLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
    if(pnt1.x() > pnt2.x()) {
        qSwap(pnt1, pnt2);
    }
    for(int i = pnt1.x(); i < pnt2.x(); i ++) {
        setPixelSafe(QPoint(i, pnt1.y()), clr);
    }
}
void Calculator::drawVLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
    if(pnt1.y() > pnt2.y()) {
        qSwap(pnt1, pnt2);
    }
    for(int i = pnt1.y(); i < pnt2.y(); i ++) {
        setPixelSafe(QPoint(pnt1.x(), i), clr);
    }
}
void Calculator::setPixelSafe(int x_coord, int y_coord, QRgb color) {
    if((0 > x_coord) || (scene.width() - 1 < x_coord)) {
        return;
    }
    if((0 > y_coord) || (scene.height() - 1 < y_coord)) {
        return;
    }
    scene.setPixel(x_coord, y_coord, color);
    return;
}
void Calculator::setPixelSafe(QPoint pnt, QRgb color) {
    setPixelSafe(pnt.x(), pnt.y(), color);
}
QImage Calculator::drawSimpleTeapot() {

    mutex.lock();
        int NUM_SEGMENTS = this->NUM_SEGMENTS;
        QSize scene_size = this->scene_size;
        QPoint start_point = this->start_point;
        QPoint end_point = this->end_point;
        bool draw_axis_on = this->draw_axis_on;
        //bool draw_box_on = this->draw_box_on;
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

    QVector<QVector3D> bezier_surface;
    for(int ptch = 0; ptch < patch_number; ptch++) {
        for(double u = 0.0; u <= 1.0; u += 1.0) {
            for(double v = 0.0; v <= 1.0; v += 1.0) {
                QVector3D surf_point(0, 0, 0);
                for(int i = 0; i <= dimension1; i++) {
                    for(int j = 0; j <= dimension2; j++) {
                        surf_point += bernstein(dimension1, i, u) * bernstein(dimension2, j, v) * point_set.at(ptch * (dimension1 + 1) * (dimension2 + 1) + j * (dimension1 + 1) + i);
                    }
                }
                bezier_surface.append(surf_point);
                checkRangeForBox(surf_point);
                surf_point = QVector3D(0.0, 0.0, 0.0);
            }
        }
    }

    for(int i = 0; i < patch_number; i++) {
        drawLine(project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4))), project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4 + 1))), teapot_color);
        drawLine(project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4 + 1))), project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4 + 3))), teapot_color);
        drawLine(project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4 + 2))), project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4 + 3))), teapot_color);
        drawLine(project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4 + 2))), project3Dto2Dscreen(matr_rot.mapVector(bezier_surface.at(i * 4))), teapot_color);
    }
    return scene;
}
