#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calculator.h"
#include <QResizeEvent>

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
    void resizeEvent(QResizeEvent *e);

public slots:
    void drawProjection(QImage);
    
private:
    Ui::MainWindow *ui;
    QImage projection;
    //Calculator calculator;
};

#endif // MAINWINDOW_H
