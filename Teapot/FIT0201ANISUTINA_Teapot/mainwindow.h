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
    void resizeEvent(QResizeEvent *e);
    void loadListOfFiles();

private slots:
    void on_axisFlag_clicked();
    void updateProjection(QString);

    void on_boxFlag_clicked();

    void on_scale_flag_clicked();

private:
    Ui::MainWindow *ui;
    QStringList files_names;
};

#endif // MAINWINDOW_H
