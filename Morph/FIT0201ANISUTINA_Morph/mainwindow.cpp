#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <qmath.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = QImage(600, 400, QImage::Format_RGB888);
    baseTexture = QImage(":/baseTexture.png");
    texel = QSize(4, 4);
    initializePoints();
    constructMipLevels();
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
    scene = QImage(e->size().width() - 90, e->size().height() - 40, QImage::Format_RGB888);
    redrawScene();
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

void MainWindow::redrawScene() {
    scene.fill(QColor(255, 255, 255).rgba());
    drawBorders();
    coefficientsForTopTransform = gausSolver(fillTopMatrix(f_point_current, c_point_current));
    coefficientsForBottomTransform = gausSolver(fillBottomMatrix(f_point_current, c_point_current));
    renderingTexturedImage();
    update();
}

void MainWindow::on_fPointSlider_sliderMoved(int position) {
    f_point_current = QPoint(f_point.x() + position, f_point.y());
    redrawScene();
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
    bool isLayerNearest = ui->nearestLayerBtn->isChecked();
    bool isLayerLinear = ui->linearLayerBtn->isChecked();
    bool isMipNone = ui->noneMipBtn->isChecked();

    if(isLayerNearest) {
        renderLayerNearestFilteredImage();
    }
    if(isLayerLinear) {
        renderLayerLinearFilteredImage();
    }
    if(isMipNone) {
        renderMipNoneFilteredImage();
    }
}

QRgb MainWindow::layerNearestFilter(qreal horz_coord, qreal vert_coord) {
    horz_coord  *= baseTexture.width();
    vert_coord  *= baseTexture.height();
    int x_coord = (horz_coord / texel.width()) * texel.width() + texel.width() / 2;
    int y_coord = (vert_coord / texel.height()) * texel.height() + texel.height() / 2;
    if (pixelSafe(x_coord, y_coord, baseTexture.width(), baseTexture.height())) {
        return baseTexture.pixel(x_coord, y_coord);
    }
   return QRgb(0);
}

QRgb MainWindow::layerNearestFilter(QVector<qreal> coordinates) {
    return layerNearestFilter(coordinates.first(), coordinates.last());
}

QRgb MainWindow::layerLinearFilter(qreal horz_coord1, qreal vert_coord1) {
    horz_coord1  *= baseTexture.width();
    vert_coord1  *= baseTexture.height();
    int horz_coord = qRound(horz_coord1);
    int vert_coord = qRound(vert_coord1);
    int dist_w = horz_coord % texel.width();
    int dist_h = vert_coord % texel.height();
    double a_distance = 0.0;
    double b_distance = 0.0;
    //get 4 texels
    QPoint A_texel = QPoint(texel.width() / 2, texel.height() / 2);
    QPoint B_texel;
    QPoint C_texel;
    QPoint D_texel;
    QRgb M_color;
    QRgb N_color;
    QRgb O_color;
    if((texel.width() / 2) > dist_w) {
        if((texel.height() / 2) > dist_h) {
            if((0 != horz_coord / texel.width()) && (0 != horz_coord / texel.height())) {
                A_texel = QPoint((horz_coord / texel.width()) * texel.width() - texel.width() / 2, (vert_coord / texel.height()) * texel.height() - texel.height() / 2) ;
            }
            a_distance = (dist_w + texel.width() / 2) / texel.width();
            b_distance = (dist_h + texel.height() / 2)  / texel.height();
            B_texel = QPoint(A_texel.x() + texel.width(), A_texel.y());
            C_texel = QPoint(A_texel.x(), A_texel.y() + texel.height());
            D_texel = QPoint(A_texel.x() + texel.width(), A_texel.y() + texel.height());
            if(pixelSafe(A_texel.x(), A_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(B_texel.x(), B_texel.y(), baseTexture.width(), baseTexture.height())) {
                M_color = (1.0 - a_distance) * baseTexture.pixel(A_texel) + a_distance * baseTexture.pixel(B_texel);
            }
            if(pixelSafe(C_texel.x(), C_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(D_texel.x(), D_texel.y(), baseTexture.width(), baseTexture.height())) {
                N_color = (1.0 - a_distance) * baseTexture.pixel(C_texel) + a_distance * baseTexture.pixel(D_texel);
            }
            O_color = (1 - b_distance) * M_color + b_distance * N_color;
        } else {
            if(0 != horz_coord / texel.width()) {
                A_texel = QPoint((horz_coord / texel.width()) * texel.width() - texel.width() / 2, (vert_coord / texel.height()) * texel.height());
            }
            a_distance = (dist_w + texel.width() / 2) / texel.width();
            b_distance = (texel.height() - (dist_h - texel.height() / 2)) / texel.height();
            B_texel = QPoint(A_texel.x() + texel.width(), A_texel.y());
            C_texel = QPoint(A_texel.x(), A_texel.y() + texel.height());
            D_texel = QPoint(A_texel.x() + texel.width(), A_texel.y() + texel.height());
            if(pixelSafe(A_texel.x(), A_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(B_texel.x(), B_texel.y(), baseTexture.width(), baseTexture.height())) {
                M_color = a_distance * baseTexture.pixel(A_texel) + (1 - a_distance) * baseTexture.pixel(B_texel);
            }
            if(pixelSafe(C_texel.x(), C_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(D_texel.x(), D_texel.y(), baseTexture.width(), baseTexture.height())) {
                N_color = a_distance * baseTexture.pixel(C_texel) + (1 - a_distance) * baseTexture.pixel(D_texel);
            }
            O_color = (1 - b_distance) * N_color + b_distance * M_color;
        }
    } else {
        if((texel.height() / 2) > dist_h) {
            if(0 != horz_coord / texel.height()) {
                A_texel = QPoint((horz_coord / texel.width()) * texel.width(), (vert_coord / texel.height()) * texel.height() - texel.height() / 2) ;
            }
            a_distance = (texel.width() - (dist_w - texel.width() / 2)) / texel.width();
            b_distance = (dist_h + texel.height() / 2)  / texel.height();
            B_texel = QPoint(A_texel.x() + texel.width(), A_texel.y());
            C_texel = QPoint(A_texel.x(), A_texel.y() + texel.height());
            D_texel = QPoint(A_texel.x() + texel.width(), A_texel.y() + texel.height());
            if(pixelSafe(A_texel.x(), A_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(B_texel.x(), B_texel.y(), baseTexture.width(), baseTexture.height())) {
                M_color = a_distance * baseTexture.pixel(A_texel) + (1 - a_distance) * baseTexture.pixel(B_texel);
            }
            if(pixelSafe(C_texel.x(), C_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(D_texel.x(), D_texel.y(), baseTexture.width(), baseTexture.height())) {
                N_color = a_distance * baseTexture.pixel(C_texel) + (1 - a_distance) * baseTexture.pixel(D_texel);
            }
            O_color = (1 - b_distance) * M_color + b_distance * N_color;
        } else {
            A_texel = QPoint((horz_coord / texel.width()) * texel.width(), (vert_coord / texel.height()) * texel.height());
            a_distance = (texel.width() - (dist_w - texel.width() / 2)) / texel.width();
            b_distance = (texel.height() - (dist_h - texel.height() / 2) / texel.height());
            B_texel = QPoint(A_texel.x() + texel.width(), A_texel.y());
            C_texel = QPoint(A_texel.x(), A_texel.y() + texel.height());
            D_texel = QPoint(A_texel.x() + texel.width(), A_texel.y() + texel.height());
            if(pixelSafe(A_texel.x(), A_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(B_texel.x(), B_texel.y(), baseTexture.width(), baseTexture.height())) {
                M_color = a_distance * baseTexture.pixel(A_texel) + (1 - a_distance) * baseTexture.pixel(B_texel);
            }
            if(pixelSafe(C_texel.x(), C_texel.y(), baseTexture.width(), baseTexture.height()) && pixelSafe(D_texel.x(), D_texel.y(), baseTexture.width(), baseTexture.height())) {
                N_color = a_distance * baseTexture.pixel(C_texel) + (1 - a_distance) * baseTexture.pixel(D_texel);
            }
            O_color = (1 - b_distance) * N_color + b_distance * M_color;
        }
    }
    return O_color;
}

QRgb MainWindow::layerLinearFilter(QVector<qreal> coordinates) {
    return layerLinearFilter(coordinates.first(), coordinates.last());
}

QVector<qreal> MainWindow::gausSolver(QVector<QVector<qreal> > slae) {

    QVector<qreal> coeffTransform;
    //1st pass to make upper triangular matrix
    for(int i = 0; i < slae.size(); i++) {
        if(0 == slae[i][i]) {
            for(int j = i + 1 ; j < slae.size(); j++) {
                if(slae[j][i] != 0) {
                    slae[i].swap(slae[j]);
                    break;
                }
            }
        }
        qreal divisor = slae[i][i];
        if(0 != divisor) {
            for(int j = i; j < slae[i].size(); j++) {
                slae[i][j] /= divisor;
            }
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
        coeffTransform.append(slae[i][slae[i].size() - 1]);
    }
    return coeffTransform;
}

QVector<QVector<qreal> > MainWindow::fillTopMatrix(int x1_star, int x2_star) {
    //1st row
    QVector<QVector<qreal> > slae;
    QVector<qreal> rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(2, 1.0);
    slae.append(rowMatrix);
    //2nd row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(5, 1.0);
    slae.append(rowMatrix);
    //3d row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, 256.0);
    rowMatrix.replace(2, 1.0);
    rowMatrix.replace(6, -256.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //4th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 256.0);
    rowMatrix.replace(5, 1.0);
    slae.append(rowMatrix);
    //5th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, static_cast<qreal>(x1_star));
    rowMatrix.replace(1, 128.0);
    rowMatrix.replace(2, 1.0);
    slae.append(rowMatrix);
    //6th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 2 * static_cast<qreal>(x1_star));
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 2.0);
    rowMatrix.replace(6, -static_cast<qreal>(x1_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //7th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, static_cast<qreal>(x2_star));
    rowMatrix.replace(1, 128.0);
    rowMatrix.replace(2, 1.0);
    rowMatrix.replace(6, -static_cast<qreal>(x2_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //8th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 2 * static_cast<qreal>(x2_star));
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 2.0);
    rowMatrix.replace(6, -static_cast<qreal>(x2_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    return slae;
}

QVector<QVector<qreal> > MainWindow::fillBottomMatrix(int x1_star, int x2_star) {
    //1st row
    QVector<QVector<qreal> > slae;
    QVector<qreal> rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, static_cast<qreal>(x1_star));
    rowMatrix.replace(1, 128.0);
    rowMatrix.replace(2, 1.0);
    slae.append(rowMatrix);
    //2nd row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 2 * static_cast<qreal>(x1_star));
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 2.0);
    rowMatrix.replace(6, -static_cast<qreal>(x1_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //3d row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, static_cast<qreal>(x2_star));
    rowMatrix.replace(1, 128.0);
    rowMatrix.replace(2, 1.0);
    rowMatrix.replace(6, -static_cast<qreal>(x2_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //4th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 2 * static_cast<qreal>(x2_star));
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 2.0);
    rowMatrix.replace(6, -static_cast<qreal>(x2_star));
    rowMatrix.replace(7, -128.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //5th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(1, 256.0);
    rowMatrix.replace(2, 1.0);
    slae.append(rowMatrix);
    //6th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 1.0);
    rowMatrix.replace(7, -256.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //7th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(0, 256.0);
    rowMatrix.replace(1, 256.0);
    rowMatrix.replace(2, 1.0);
    rowMatrix.replace(6, -256.0);
    rowMatrix.replace(7, -256.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    //8th row
    rowMatrix = QVector<qreal>(9, 0.0);
    rowMatrix.replace(3, 256.0);
    rowMatrix.replace(4, 256.0);
    rowMatrix.replace(5, 1.0);
    rowMatrix.replace(6, -256.0);
    rowMatrix.replace(7, -256.0);
    rowMatrix.replace(8, 1.0);
    slae.append(rowMatrix);
    return slae;
}

QVector<QVector<qreal> > MainWindow::fillTopMatrix(QPoint f_pnt, QPoint c_pnt) {
    return fillTopMatrix(f_pnt.x(), c_pnt.x());
}

QVector<QVector<qreal> > MainWindow::fillBottomMatrix(QPoint f_pnt, QPoint c_pnt) {
    return fillBottomMatrix(f_pnt.x(), c_pnt.x());
}

QPoint MainWindow::relativeCoordinates(QPoint pnt) {
    return QPoint(pnt.x() - (scene.width() / 2 - 128), pnt.y() - (scene.height() / 2 - 128));
}

void MainWindow::initializePoints() {
    a_point = QPoint(0, 0);
    b_point = QPoint(256, 0);
    c_point = QPoint(256, 128);
    f_point = QPoint(0, 128);
    e_point = QPoint(0, 256);
    d_point = QPoint(256, 256);
    c_point_current = c_point;
    f_point_current = f_point;
}

void MainWindow::drawBorders() {
    setPoint(toSceneCoordinates(a_point));
    setPoint(toSceneCoordinates(b_point));
    setPoint(toSceneCoordinates(c_point_current));
    setPoint(toSceneCoordinates(d_point));
    setPoint(toSceneCoordinates(e_point));
    setPoint(toSceneCoordinates(f_point_current));
    drawLine(toSceneCoordinates(a_point), toSceneCoordinates(b_point), QColor(0, 0, 0).rgba());
    drawLine(toSceneCoordinates(b_point), toSceneCoordinates(c_point_current), QColor(0, 0, 0).rgba());
    drawLine(toSceneCoordinates(c_point_current), toSceneCoordinates(d_point), QColor(0, 0, 0).rgba());
    drawLine(toSceneCoordinates(d_point), toSceneCoordinates(e_point), QColor(0, 0, 0).rgba());
    drawLine(toSceneCoordinates(e_point), toSceneCoordinates(f_point_current), QColor(0, 0, 0).rgba());
    drawLine(toSceneCoordinates(f_point_current), toSceneCoordinates(a_point), QColor(0, 0, 0).rgba());
}

QVector<qreal> MainWindow::getTexturePoint(QPoint pnt, QVector<qreal> coeffs) {
    qreal u_component = (pnt.x() * coeffs[0] + pnt.y() * coeffs[1] + coeffs[2]) / (pnt.x() * coeffs[6] + pnt.y() * coeffs[7] + 1);
    qreal v_component = (pnt.x() * coeffs[3] + pnt.y() * coeffs[4] + coeffs[5]) / (pnt.x() * coeffs[6] + pnt.y() * coeffs[7] + 1);
    u_component = qAbs(u_component);
    v_component = qAbs(v_component);
    if (1.0 < u_component) {
        u_component = 2.0 - u_component;
    }
    if (1.0 < v_component) {
        v_component = 2.0 - v_component;
    }
    QVector<qreal> coordinates;
    coordinates.append(u_component);
    coordinates.append(v_component);
    return coordinates;
}

QPoint MainWindow::toSceneCoordinates(QPoint pnt) {
    return QPoint((scene.width() / 2 - 128) + pnt.x(), (scene.height() / 2 - 128) + pnt.y());
}

QVector<QPoint> MainWindow::getPointsOfBrezenhemLine(QPoint pnt1, QPoint pnt2, QVector<QPoint> line) {
    int curr_x = pnt1.x(), curr_y = pnt1.y(), end_x = pnt2.x(), end_y = pnt2.y();

    int deltaX = qAbs(end_x - curr_x);
    int deltaY = qAbs(end_y - curr_y);
    int signX = curr_x < end_x ? 1 : -1;
    int signY = curr_y < end_y ? 1 : -1;
    int err = deltaX - deltaY;
    while((end_x != curr_x) || (end_y != curr_y)) {
        line.append(relativeCoordinates(QPoint(curr_x, curr_y)));//append to vector of points
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
    line.append(relativeCoordinates(QPoint(end_x, end_y)));
    return line;
}

void MainWindow::renderLayerNearestFilteredImage() {
    QVector<qreal> topCoeffs = coefficientsForTopTransform;
    QVector<qreal> bottomCoeffs = coefficientsForBottomTransform;
    QVector<QPoint> af_line;
    QVector<QPoint> bc_line;
    af_line = getPointsOfBrezenhemLine(toSceneCoordinates(a_point), toSceneCoordinates(f_point_current), af_line);
    bc_line = getPointsOfBrezenhemLine(toSceneCoordinates(b_point), toSceneCoordinates(c_point_current), bc_line);
    int t = 0;
    int g = 0;
    for(int h = 0; h < 128; h++) {
        while (h == af_line[t + 1].y()) {
            t++;
        }
        while (h == bc_line[g + 1].y()) {
            g++;
        }
        for(int w = af_line[t].x(); w < bc_line[g].x(); w++) {
            QRgb color = layerNearestFilter(getTexturePoint(QPoint(w, h), topCoeffs));
            setPixelSafe(toSceneCoordinates(QPoint(w, h)), color);
        }
        t++;
        g++;
    }
    QVector<QPoint> ef_line;
    QVector<QPoint> cd_line;
    ef_line = getPointsOfBrezenhemLine(toSceneCoordinates(f_point_current), toSceneCoordinates(e_point), ef_line);
    cd_line = getPointsOfBrezenhemLine(toSceneCoordinates(c_point_current), toSceneCoordinates(d_point), cd_line);
    t = 0;
    g = 0;
    for(int h = 128; h < 256; h++) {
        while (h == ef_line[t + 1].y()) {
            t++;
        }
        while (h == cd_line[g + 1].y()) {
            g++;
        }
        for(int w = ef_line[t].x(); w < cd_line[g].x(); w++) {
            QRgb color = layerNearestFilter(getTexturePoint(QPoint(w, h), bottomCoeffs));
            setPixelSafe(toSceneCoordinates(QPoint(w, h)), color);
        }
        t++;
        g++;
    }
}

void MainWindow::renderLayerLinearFilteredImage() {
    QVector<qreal> topCoeffs = coefficientsForTopTransform;
    QVector<qreal> bottomCoeffs = coefficientsForBottomTransform;
    QVector<QPoint> af_line;
    QVector<QPoint> bc_line;
    af_line = getPointsOfBrezenhemLine(toSceneCoordinates(a_point), toSceneCoordinates(f_point_current), af_line);
    bc_line = getPointsOfBrezenhemLine(toSceneCoordinates(b_point), toSceneCoordinates(c_point_current), bc_line);
    int t = 0;
    int g = 0;
    for(int h = 0; h < 128; h++) {
        while (h == af_line[t + 1].y()) {
            t++;
        }
        while (h == bc_line[g + 1].y()) {
            g++;
        }
        for(int w = af_line[t].x(); w < bc_line[g].x(); w++) {
            QRgb color = layerLinearFilter(getTexturePoint(QPoint(w, h), topCoeffs));
            setPixelSafe(toSceneCoordinates(QPoint(w, h)), color);
        }
        t++;
        g++;
    }
    QVector<QPoint> ef_line;
    QVector<QPoint> cd_line;
    ef_line = getPointsOfBrezenhemLine(toSceneCoordinates(f_point_current), toSceneCoordinates(e_point), ef_line);
    cd_line = getPointsOfBrezenhemLine(toSceneCoordinates(c_point_current), toSceneCoordinates(d_point), cd_line);
    t = 0;
    g = 0;
    for(int h = 128; h < 256; h++) {
        while (h == ef_line[t + 1].y()) {
            t++;
        }
        while (h == cd_line[g + 1].y()) {
            g++;
        }
        for(int w = ef_line[t].x(); w < cd_line[g].x(); w++) {
            QRgb color = layerLinearFilter(getTexturePoint(QPoint(w, h), bottomCoeffs));
            setPixelSafe(toSceneCoordinates(QPoint(w, h)), color);
        }
        t++;
        g++;
    }
}

void MainWindow::on_nearestLayerBtn_clicked() {
    renderingTexturedImage();
    update();
}

void MainWindow::on_linearLayerBtn_clicked() {
    renderingTexturedImage();
    update();
}

void MainWindow::on_noneMipBtn_clicked(){
    renderingTexturedImage();
    update();
}

void MainWindow::constructMipLevels() {
    mipLevels.append(baseTexture);
    int old_height = mipLevels[0].height();
    int last_level = 0;
    while(1 <= old_height / 2) {
        QImage mipLevel = QImage(old_height / 2, old_height / 2, QImage::Format_RGB888);
        for(int w = 0; w < old_height / 2; w++) {
            for(int h = 0; h < old_height / 2; h++) {
                QRgb color = mipLevels[last_level].pixel(2 * w, 2 * h);
                mipLevel.setPixel(w, h, color);
            }
        }
        mipLevels.append(mipLevel);
        last_level++;
        old_height /= 2;
    }
}

void MainWindow::renderMipNoneFilteredImage() {
    QVector<qreal> topCoeffs = coefficientsForTopTransform;
    QVector<qreal> bottomCoeffs = coefficientsForBottomTransform;
    QVector<QPoint> af_line;
    QVector<QPoint> bc_line;
    af_line = getPointsOfBrezenhemLine(toSceneCoordinates(a_point), toSceneCoordinates(f_point_current), af_line);
    bc_line = getPointsOfBrezenhemLine(toSceneCoordinates(b_point), toSceneCoordinates(c_point_current), bc_line);
    int t = 0;
    int g = 0;
    for(int h = 0; h < 128; h++) {
        while (h == af_line[t + 1].y()) {
            t++;
        }
        while (h == bc_line[g + 1].y()) {
            g++;
        }
        for(int w = af_line[t].x(); w < bc_line[g].x(); w++) {
            QVector<QVector<qreal> > coordinates;
            coordinates.append(getTexturePoint(QPoint(w, h), topCoeffs));
            coordinates.append(getTexturePoint(QPoint(w + 1, h), topCoeffs));
            coordinates.append(getTexturePoint(QPoint(w, h + 1), topCoeffs));
            coordinates.append(getTexturePoint(QPoint(w + 1, h + 1), topCoeffs));
            QRgb color = mipNoneFilter(coordinates);
            setPixelSafe(toSceneCoordinates(QPoint(w, h)), color);
        }
        t++;
        g++;
    }
    QVector<QPoint> ef_line;
    QVector<QPoint> cd_line;
    ef_line = getPointsOfBrezenhemLine(toSceneCoordinates(f_point_current), toSceneCoordinates(e_point), ef_line);
    cd_line = getPointsOfBrezenhemLine(toSceneCoordinates(c_point_current), toSceneCoordinates(d_point), cd_line);
    t = 0;
    g = 0;
    for(int h = 128; h < 256; h++) {
        while (h == ef_line[t + 1].y()) {
            t++;
        }
        while (h == cd_line[g + 1].y()) {
            g++;
        }
        for(int w = ef_line[t].x(); w < cd_line[g].x(); w++) {
            QVector<QVector<qreal> > coordinates;
            coordinates.append(getTexturePoint(QPoint(w, h), bottomCoeffs));
            coordinates.append(getTexturePoint(QPoint(w + 1, h), bottomCoeffs));
            coordinates.append(getTexturePoint(QPoint(w, h + 1), bottomCoeffs));
            coordinates.append(getTexturePoint(QPoint(w + 1, h + 1), bottomCoeffs));
            QRgb color = mipNoneFilter(coordinates);
            setPixelSafe(toSceneCoordinates(QPoint(w, h)), color);
        }
        t++;
        g++;
    }
}

QRgb MainWindow::mipNoneFilter(QVector<QVector<qreal> > coordinatesSet) {
    double dist1 = qPow(coordinatesSet[0][0] - coordinatesSet[1][0], 2) + qPow(coordinatesSet[0][1] - coordinatesSet[1][1], 2);
    double dist2 = qPow(coordinatesSet[1][0] - coordinatesSet[2][0], 2) + qPow(coordinatesSet[1][1] - coordinatesSet[2][1], 2);
    double dist3 = qPow(coordinatesSet[2][0] - coordinatesSet[3][0], 2) + qPow(coordinatesSet[2][1] - coordinatesSet[3][1], 2);
    double dist4 = qPow(coordinatesSet[3][0] - coordinatesSet[0][0], 2) + qPow(coordinatesSet[3][1] - coordinatesSet[0][1], 2);
    double distance = qSqrt(qMax(qMax(dist1, dist2), qMax(dist3, dist4)));
    double min_differ = qAbs(distance * mipLevels[0].height() - 1.0);
    int index_min_differ = 0;
    for(int i = 1; i < mipLevels.size() - 1; i++) {
        if(qAbs(distance * mipLevels[i].height() - 1.0) < min_differ) {
            min_differ = qAbs(distance * mipLevels[i].height() - 1.0);
            index_min_differ = i;
        }
    }
    if(distance * mipLevels[index_min_differ].height() > 1.0) {
        index_min_differ++;
    }
    int horz_coord = static_cast<int>(coordinatesSet[0][0] * mipLevels[index_min_differ].width() - .5);
    int vert_coord = static_cast<int>(qRound(coordinatesSet[0][1] * mipLevels[index_min_differ].height()) - .5);

    if(pixelSafe(horz_coord, vert_coord, mipLevels[index_min_differ].width(), mipLevels[index_min_differ].height())) {
        return mipLevels[index_min_differ].pixel(horz_coord, vert_coord);
    }
    return QRgb(0);
}
