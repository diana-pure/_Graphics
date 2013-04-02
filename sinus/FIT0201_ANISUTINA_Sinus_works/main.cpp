#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("FIT0201ANISUTINA_Sinus");
    w.setFixedSize(400, 280);
    w.show();
    
    return a.exec();
}
