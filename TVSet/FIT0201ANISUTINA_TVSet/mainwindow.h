#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "interpretator.h"
#include <QMainWindow>
#include <QLabel>

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
    void loadListOfFiles();


protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

public slots:
    void updateImage();
    void updateImage(QImage);
    void updateImage(QString);
    void restoreDefaultScale();

private slots:
    void on_BT601Standart_released();
    void on_BT709Standart_released();
    void refreshGamma();
    void on_gammaSlider_valueChanged(int value);
    void scaleImage();

private:
    Ui::MainWindow *ui;
    QImage image;
    QImage scaled;
    QLabel *pImage_lbl; //label used to contain image in scroll area (like at the example from qt docs)
    QStringList files_names;
    Interpretator interpret;
    int current_scale;
    int default_scale;
};

#endif // MAINWINDOW_H
