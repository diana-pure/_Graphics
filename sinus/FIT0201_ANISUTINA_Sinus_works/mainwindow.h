#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define FIELD_WIDTH 400
#define FIELD_HEIGTH 200
#define STEP 1

#include <QMainWindow>
#include <QImage>
#include <QPainter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_clearButton_clicked();
    void paintEvent(QPaintEvent *);

    void on_drawSinusButton_clicked();

    void on_drawAxisButton_clicked();

signals:
    void abc();
    void abc1();

private:
    Ui::MainWindow *ui;
    QImage* field;
    //QPainter painter;
};

#endif // MAINWINDOW_H
