#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = QImage(600, 400, QImage::Format_RGB888);
    redrawScene();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawImage(0, 0, scene);
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    scene = QImage(e->size().width(), e->size().height() - 40, QImage::Format_RGB888);
    clearScene();
    redrawScene();
}

void MainWindow::clearScene() {
    scene.fill(QColor(255, 255, 255).rgba());
    QSize size = scene.size();
    a_point = QPoint(size.width() / 2 - 128, size.height() / 2 - 128);
    b_point = QPoint(size.width() / 2 + 128, size.height() / 2 - 128);
    c_point = QPoint(size.width() / 2 + 128, size.height() / 2);
    f_point = QPoint(size.width() / 2 - 128, size.height() / 2);
    e_point = QPoint(size.width() / 2 - 128, size.height() / 2 + 128);
    d_point = QPoint(size.width() / 2 + 128, size.height() / 2 + 128);
    c_point_current = c_point;
    f_point_current = f_point;

    setPoint(a_point);
    setPoint(b_point);
    setPoint(c_point_current);
    setPoint(d_point);
    setPoint(e_point);
    setPoint(f_point_current);
    update();
}

void MainWindow::setPoint(QPoint pnt) {
    for(int i = -2; i < 3; i++) {
        for(int j = -2; j < 3; j++) {
            setPixelSafe(pnt.x() + i, pnt.y() + j, QColor(0, 0, 0).rgba());
        }
    }
}

void MainWindow::setPixelSafe(int x_coord, int y_coord, QRgb color) {
    if((0 > x_coord) || (scene.width() - 1 < x_coord)) {
        return;
    }
    if((0 > y_coord) || (scene.height() - 1 < y_coord)) {
        return;
    }
    scene.setPixel(x_coord, y_coord, color);
}

void MainWindow::setPixelSafe(QPoint pnt, QRgb color) {
    setPixelSafe(pnt.x(), pnt.y(), color);
}

void MainWindow::on_fPointSlider_sliderMoved(int position) {
    f_point_current = QPoint(f_point.x() + position, f_point.y());
    redrawScene();
}

void MainWindow::redrawScene() {
    scene.fill(QColor(255, 255, 255).rgba());
    setPoint(a_point);
    setPoint(b_point);
    setPoint(c_point_current);
    setPoint(d_point);
    setPoint(e_point);
    setPoint(f_point_current);
    drawLine(a_point, b_point, QColor(0, 0, 0).rgba());
    drawLine(b_point, c_point_current, QColor(0, 0, 0).rgba());
    drawLine(c_point_current, d_point, QColor(0, 0, 0).rgba());
    drawLine(d_point, e_point, QColor(0, 0, 0).rgba());
    drawLine(e_point, f_point_current, QColor(0, 0, 0).rgba());
    drawLine(f_point_current, a_point, QColor(0, 0, 0).rgba());
    update();
}

void MainWindow::on_cPointSlider_sliderMoved(int position) {
    c_point_current = QPoint(c_point.x() + position, c_point.y());
    redrawScene();
}

void MainWindow::drawLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
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
    while((end_x != curr_x) || (end_y != curr_y)) {
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
void MainWindow::drawHLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
    if(pnt1.x() > pnt2.x()) {
        qSwap(pnt1, pnt2);
    }
    for(int i = pnt1.x(); i < pnt2.x(); i ++) {
        setPixelSafe(QPoint(i, pnt1.y()), clr);
    }
}
void MainWindow::drawVLine(QPoint pnt1, QPoint pnt2, QRgb clr) {
    if(pnt1.y() > pnt2.y()) {
        qSwap(pnt1, pnt2);
    }
    for(int i = pnt1.y(); i < pnt2.y(); i ++) {
        setPixelSafe(QPoint(pnt1.x(), i), clr);
    }
}
