#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    field = new QImage(FIELD_WIDTH, FIELD_HEIGTH, QImage::Format_RGB888);
    field->fill(QColor(255, 255, 0));

    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(on_clearButton_clicked()));
    connect(ui->drawSinusButton, SIGNAL(clicked()), this, SLOT(on_drawSinusButton_clicked()));
    connect(ui->drawAxisButton, SIGNAL(clicked()), this, SLOT(on_drawAxisButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete field;
}

void MainWindow::on_clearButton_clicked()
{
    field->fill(QColor(255,255,255));
    QPainter painter(this);
    painter.drawImage(100, 100, *field);
    this->update();
}

void MainWindow::paintEvent(QPaintEvent *){

    QPainter painter(this);
    painter.drawImage(0, 0, *field);

}

void MainWindow::on_drawSinusButton_clicked()
{
    int x = 0;
    const int SHIFT = static_cast<int> (FIELD_HEIGTH/2);
    const double PI = atan(1)*4;   //PI == 3.14 (arctang from 1 is 45 degrees -> *4 we get 180 degrees -> PI)
    //const int HALF_WIDTH = static_cast<int> (FIELD_WIDTH/2);

    for(x = 0; x < FIELD_WIDTH; x += STEP){
        field->setPixel(x, 30*sin(x*PI/60) + SHIFT, QColor(255, 0, 0).rgba()); //what about casting? sin returns double
    }
    QPainter painter(this);
    painter.drawImage(0, 0, *field);
    this->update();
}

void MainWindow::on_drawAxisButton_clicked()
{
    int x = 0, y = 0;
    const int SHIFT = static_cast<int> (FIELD_HEIGTH/2);
    //const int HALF_WIDTH = static_cast<int> (FIELD_WIDTH/2);

    for(x = 0; x < FIELD_WIDTH; x += STEP){

        field->setPixel(x, SHIFT, QColor(0, 0, 255).rgba());
        if (0 == (x % 30)){
            field->setPixel(x, SHIFT - 1, QColor(0, 0, 255).rgba());
            field->setPixel(x, SHIFT - 2, QColor(0, 0, 255).rgba());
        }
    }
    for(y = 0; y < FIELD_HEIGTH; y += STEP){

        field->setPixel(180, y, QColor(0, 0, 255).rgba());
    }
    field->setPixel(181, FIELD_HEIGTH/2 - 30, QColor(0, 0, 255).rgba());
    field->setPixel(181, FIELD_HEIGTH/2 + 30, QColor(0, 0, 255).rgba());

    for(x = 180; x < 190; x++){
        field->setPixel(x, 3 * (x - 180), QColor(0, 0, 255).rgba());
        field->setPixel(180 + (180 - x), 3 * (x - 180), QColor(0, 0, 255).rgba());
    }
    for(y = 90; y < 110; y++){
        field->setPixel(3 * y + 100, y, QColor(0, 0, 255).rgba());
        field->setPixel(-3 * (y - 100) + 400, y, QColor(0, 0, 255).rgba());
    }

    QPainter painter(this);
    painter.drawImage(0, 0, *field);
    this->update();
}
