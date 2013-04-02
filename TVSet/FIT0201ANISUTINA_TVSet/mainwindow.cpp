    #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    default_scale = 1;
    current_scale = 1;
    image.load(":/aliceA.png");
    scaled = image;
    files_names = QStringList();
    pImage_lbl = new QLabel();
    pImage_lbl->setAlignment(Qt::AlignCenter);
    ui->imageArea->setWidget(pImage_lbl);
    ui->imageArea->setBackgroundRole(QPalette::NoRole);

    loadListOfFiles();

    setAcceptDrops(true);

    connect(&interpret, SIGNAL(ready(QImage)), this, SLOT(updateImage(QImage)));
    connect(ui->gammaSlider, SIGNAL(sliderReleased()), this, SLOT(refreshGamma()));
    connect(ui->zoomSlider, SIGNAL(sliderReleased()), this, SLOT(scaleImage()));
    connect(ui->filesList, SIGNAL(itemSelectionChanged()), this, SLOT(restoreDefaultScale()));
    connect(ui->filesList, SIGNAL(currentTextChanged(QString)), this, SLOT(updateImage(QString)));

    updateImage("aliceA.png");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete pImage_lbl;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
}

void MainWindow::loadListOfFiles()
{                                   //default list of files
    files_names.append("aliceA.png");
    files_names.append("aliceB.png");
    files_names.append("greenA.png");
    files_names.append("greenB.png");
    files_names.append("helloA.png");
    files_names.append("helloB.png");
    files_names.append("spiderA.png");
    files_names.append("spiderB.png");
    ui->filesList->addItems(files_names);
    ui->filesList->setCurrentRow(0);
}

void MainWindow::updateImage()
{
    pImage_lbl->setPixmap(QPixmap::fromImage(image));
    ui->imageArea->update();
}

void MainWindow::updateImage(QImage new_img)
{
    scaled = new_img;
    pImage_lbl->setPixmap(QPixmap::fromImage(scaled));
    ui->imageArea->update();
}

void MainWindow::updateImage(QString name)
{
    QString name_to_load = QString();
    if(!name.contains(":/")) //file from default list or drag-n-droped
    {
        name_to_load = QString(":/");
    }
    name_to_load.append(name);
    image.load(name_to_load);
    interpret.grabImage(image);      //interpretator's settings
    interpret.setScale(current_scale);
    interpret.recover();        //runs thread which computing original image
}

void MainWindow::refreshGamma()
{
    float new_gamma = ui->gammaSlider->value() / 10.0;
    interpret.setGamma(new_gamma);
    updateImage(ui->filesList->currentItem()->text());
}

void MainWindow::on_BT601Standart_released()
{
    interpret.setStandartCoefficients();
    updateImage(ui->filesList->currentItem()->text());
}

void MainWindow::on_BT709Standart_released()
{
    interpret.setHDCoefficients();
    updateImage(ui->filesList->currentItem()->text());
}

void MainWindow::on_gammaSlider_valueChanged(int value)
{
    ui->gammaValue->display(value / 10.0);
}

void MainWindow::scaleImage()
{
    ui->zoomValue->setNum(ui->zoomSlider->value());
    current_scale = ui->zoomSlider->value();
    updateImage(ui->filesList->currentItem()->text());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }
    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty())
    {
        return;
    }
    ui->filesList->addItem(fileName);   //added fileName into list and updateImage to apply recovering
    ui->filesList->setCurrentRow(ui->filesList->count() - 1);
    updateImage(fileName);
}

void MainWindow::restoreDefaultScale()
{
    ui->zoomSlider->setValue(default_scale);
    emit ui->zoomSlider->sliderReleased();
}
