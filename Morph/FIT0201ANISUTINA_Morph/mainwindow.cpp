#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = QImage(600, 400, QImage::Format_RGB888);
    redrawScene();
    baseTexture = QImage(":/baseTexture.png");
    texel = QSize(4, 4);

    fillMatrix(0, 256);
    gausSolver();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawImage(0, 0, scene);
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    scene = QImage(e->size().width() - 90, e->size().height() - 40, QImage::Format_RGB888);
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

bool MainWindow::pixelSafe(int x_coord, int y_coord, int image_width, int image_height) {
    if((0 > x_coord) || (image_width - 1 < x_coord)) {
        return false;
    }
    if((0 > y_coord) || (image_height - 1 < y_coord)) {
        return false;
    }
    return true;
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
    renderingTexturedImage();
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

void MainWindow::renderingTexturedImage() {
    //bypass every inner point
    //setPixel(i, j, layerNearestFilter(i, j))
    for(int i = a_point.x(); i < b_point.x(); i++) {
        for(int j = a_point.y(); j < d_point.y(); j++) {
            QRgb color = layerNearestFilter(i - a_point.x(), j - a_point.y());
            setPixelSafe(i, j, color);
        }
    }
/*
    for(int i = a_point.x(); i < b_point.x(); i++) {
        for(int j = a_point.y(); j < d_point.y(); j++) {
            QRgb color = layerLinearFilter(i - a_point.x(), j - a_point.y());
            setPixelSafe(i, j, color);
        }
    }
    */
}

QRgb MainWindow::layerNearestFilter(int horz_coord, int vert_coord) {
    int x_coord = horz_coord * texel.width() + texel.width() / 2;
    int y_coord = vert_coord * texel.height() + texel.height() / 2;
    if (pixelSafe(x_coord, y_coord, baseTexture.width(), baseTexture.height())) {
        return baseTexture.pixel(x_coord, y_coord);
    }
   return 0;//QRgb(0, 0, 0); // is it ok?
}

QRgb MainWindow::layerLinearFilter(int horz_coord, int vert_coord) {
    QPoint A_texel = QPoint((horz_coord ) * (texel.width() * 4), (vert_coord ) * (texel.height() * 4));
    QPoint B_texel = QPoint(A_texel.x() + texel.width(), A_texel.y());
    QPoint C_texel = QPoint(A_texel.x(), A_texel.y() + texel.height());
    QPoint D_texel = QPoint(A_texel.x() + texel.width(), A_texel.y() + texel.height());
    int a_distance = horz_coord % (texel.width() * 4) - (A_texel.x() + texel.width() / 2);
    int b_distance = vert_coord % (texel.height() * 4) - (A_texel.y() + texel.height() / 2);
    QRgb M_color = (1.0 - a_distance / static_cast<double>(texel.width())) * baseTexture.pixel(A_texel.x() + texel.width() / 2, A_texel.y() + texel.height() / 2) +
            (a_distance / static_cast<double>(texel.width())) * baseTexture.pixel(B_texel.x() + texel.width() / 2, B_texel.y() + texel.height() / 2);
    QRgb N_color = (1.0 - a_distance / static_cast<double>(texel.width())) * baseTexture.pixel(C_texel.x() + texel.width() / 2, C_texel.y() + texel.height() / 2) +
            (a_distance / static_cast<double>(texel.width())) * baseTexture.pixel(D_texel.x() + texel.width() / 2, D_texel.y() + texel.height() / 2);
    return (1.0 - b_distance / static_cast<double>(texel.height())) * M_color + b_distance / static_cast<double>(texel.height()) * N_color;


    return baseTexture.pixel(horz_coord / texel.width() + texel.width() / 2, vert_coord / texel.height() + texel.height() / 2);
}

void MainWindow::gausSolver() {

    //1st pass to make upper triangular matrix
    for(int i = 0; i < slae.size(); i++) {
        if(0 == slae[i][i]) {
            for(int j = i + 1 ; j < slae.size(); j++) { //i+1
                if(slae[j][i] != 0) {
                    slae[i].swap(slae[j]);
                    break;
                }
            }
        }
        qreal divisor = slae[i][i];
        if(0.0 != divisor) {
        for(int j = i; j < slae[i].size(); j++) {
            slae[i][j] /= divisor;
        }
        } else {
            qDebug() << "divisor is 0" << i;
        }
        for(int k = i + 1; k < slae.size(); k++) {
            qreal multiplier = slae[k][i];
            for(int l = i; l < slae[k].size(); l++) {
                slae[k][l] -= slae[i][l] * multiplier;
            }
        }
    }
    //back pass to make the identity matrix
    for(int i = slae.size() - 1; i > 0; i--) {
        for(int j = i - 1; j >= 0; j--) {
            qreal multiplier = slae[j][i];
            slae[j][slae[j].size() - 1] -= slae[i][slae[i].size() - 1] * multiplier;
            slae[j][i] = 0.0;
        }
    }
    for(int i = 0; i < slae.size(); i++) {
        qDebug() << slae[i][slae[i].size() - 1];
    }
    return;
}

void MainWindow::fillMatrix(int x1_star, int x2_star) {

    QVector<qreal> rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(2, 1.0);
    slae.append(rowMatrix);

    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(5, 1.0);
    slae.append(rowMatrix);

    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, 256.0);
    rowMatrix.replace(2, 1.0);
    rowMatrix.replace(6, -256.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);

    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 256.0);
    rowMatrix.replace(5, 1.0);
    slae.append(rowMatrix);

    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, static_cast<qreal>(x1_star));
    rowMatrix.replace(1, 128.0);
    rowMatrix.replace(2, 1.0);
    slae.append(rowMatrix);

    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 2 * static_cast<qreal>(x1_star));
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 2.0);
    rowMatrix.replace(6, -static_cast<qreal>(x1_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);

    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, static_cast<qreal>(x2_star));
    rowMatrix.replace(1, 128.0);
    rowMatrix.replace(2, 1.0);
    rowMatrix.replace(6, -static_cast<qreal>(x2_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);

    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 2 * static_cast<qreal>(x2_star));
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 2.0);
    rowMatrix.replace(6, -static_cast<qreal>(x2_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
}
