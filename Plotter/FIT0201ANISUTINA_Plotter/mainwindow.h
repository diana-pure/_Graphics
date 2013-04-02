#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void resizeEvent(QResizeEvent *);
    
public slots:
    void setFocus1Value(QPoint);
    void setFocus2Value(QPoint);

private slots:
    void on_radiusSlider_valueChanged(int);

    void on_resetScaleButton_clicked();

signals:
    void radiusChanged(int);
    void windowWasResized(QSize);

private:
    Ui::MainWindow *ui;
    int cassiniStep;
};

#endif // MAINWINDOW_H
