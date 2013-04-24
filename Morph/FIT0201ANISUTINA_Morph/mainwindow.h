#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QPoint>

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
    void resizeEvent(QResizeEvent *);
    void clearScene();
    void setPoint(QPoint);
    void setPixelSafe(int, int, QRgb);
    void setPixelSafe(QPoint, QRgb);
    bool pixelSafe(int x_coord, int y_coord, int image_width, int image_height);
    void redrawScene();
    void drawLine(QPoint, QPoint, QRgb);
    void drawHLine(QPoint, QPoint, QRgb);
    void drawVLine(QPoint, QPoint, QRgb);
    void renderingTexturedImage(QVector<qreal> topCoeffs);//, QVector<qreal> bottomCoeffs);
    QRgb layerNearestFilter(int, int);
    QRgb layerNearestFilter(QPoint);
    QRgb layerLinearFilter(int, int);
    //void gausSolver(QVector<QVector<qreal> > slae);
    QVector<qreal> gausSolver(QVector<QVector<qreal> > slae);
    QVector<QVector<qreal> > fillTopMatrix(int x1_star, int x2_star);
    QVector<QVector<qreal> > fillTopMatrix(QPoint f_pnt, QPoint c_pnt);
    QVector<QVector<qreal> > fillBottomMatrix(int x1_star, int x2_star);
    QVector<QVector<qreal> > fillBottomMatrix(QPoint f_pnt, QPoint c_pnt);
    QPoint relativeCoordinates(QPoint pnt);
    void initializePoints();
    void drawBorders();
    QPoint getTexturePoint(QPoint pnt, QVector<qreal> coeffs);
    QPoint toSceneCoordinates(QPoint);
    QVector<QPoint> getPointsOfBrezenhemLine(QPoint start_line, QPoint end_line, QVector<QPoint> line);

private slots:
    void on_fPointSlider_sliderMoved(int position);
    void on_cPointSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QImage scene;
    QPoint a_point;
    QPoint b_point;
    QPoint c_point;
    QPoint f_point;
    QPoint c_point_current;
    QPoint f_point_current;
    QPoint d_point;
    QPoint e_point;
    QImage baseTexture;
    QSize texel;
    //QVector<qreal> coeffTransform;
    //QVector<QVector<qreal> > slae;
};

#endif // MAINWINDOW_H
