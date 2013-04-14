#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = QImage(600, 400, QImage::Format_RGB888);
    clearScene();
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
    update();
}

void MainWindow::on_cPointSlider_sliderMoved(int position) {
    c_point_current = QPoint(c_point.x() + position, c_point.y());
    redrawScene();
}
