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
    void redrawScene();

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
};

#endif // MAINWINDOW_H
