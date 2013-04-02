#include "interpretator.h"
#include <QColor>
#include <qmath.h>

Interpretator::Interpretator(QObject *parent) :
    QThread(parent)
{
    scale = 1;
    gamma = 2.2;
    setStandartCoefficients();
    old_image = QImage();
    new_image = QImage();
}

Interpretator::~Interpretator(){
}

void Interpretator::run()
{
    QImage old_img = old_image, new_img = new_image;
    QImage scaled = QImage(scale * new_img.width(), scale * new_img.height(), QImage::Format_RGB888);
    bool isLuma = true, isChroma = false;
    int width = old_img.width(), height = old_img.height();
    int width2 = new_img.width();

    mutex.lock();
    if(old_img.hasAlphaChannel()){
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                QRgb color = old_img.pixel(j, i);    //for each pixel from source file: retrieving encoded information

                float Y01normed = qPow(normalize(isLuma, qAlpha(color)), gamma / 2.2);
                float U1normed =  normalize(isChroma, qRed(color));
                float Y11normed = qPow(normalize(isLuma, qGreen(color)), gamma / 2.2);
                float V1normed = normalize(isChroma, qBlue(color));
                float U2normed =  U1normed;
                float V2normed =  V1normed;
                                           //retrieving rgb components, normalising to use it in algorithm of encoding rgb
                float R1norm = (V1normed + V2normed) * (1 - Kr) + Y01normed;
                float R2norm = 2 * V1normed * (1 - Kr) + Y11normed;
                float B1norm = 2 * U1normed * (1 - Kb) + Y01normed;
                float B2norm = (U1normed + U2normed) * (1 - Kb) + Y11normed;
                float G1norm = (Y01normed - Kr * R1norm - Kb * B1norm) / (1 - Kr - Kb);
                float G2norm = (Y11normed - Kr * R2norm - Kb * B2norm) / (1 - Kr - Kb);

                QRgb color1 = qRgb(checkRgbRanges(R1norm), checkRgbRanges(G1norm), checkRgbRanges(B1norm));
                QRgb color2 = qRgb(checkRgbRanges(R2norm), checkRgbRanges(G2norm), checkRgbRanges(B2norm));


                if(inRanges(2 * j, i, width2, height))
                {
                    new_img.setPixel(2 * j, i, color1);
                }
                if(inRanges(2 * j + 1, i, width2, height))
                {
                    new_img.setPixel(2 * j + 1, i, color2);
                }
                }
            }
    }
    mutex.unlock();
                                           //scaling the image coping 1 pixel for (scale * scale) area
    int width_sc = scale * new_img.width(), height_sc = scale * new_img.height();
    if(1 != scale) {
        for(int i = 0; i < new_img.height(); i++) {
            for(int j = 0; j < new_img.width(); j++) {
                for(int sc1 = 0; sc1 < scale; sc1 ++) {
                    for(int sc2 = 0; sc2 < scale; sc2++) {
                        if(inRanges(j * scale + sc1, i * scale + sc2, width_sc, height_sc))
                        {
                            scaled.setPixel(j * scale + sc1, i * scale + sc2, QColor(new_img.pixel(j,i)).rgb());
                        }
                    }
                }
            }
        }
    } else {
        scaled = new_img;
    }
    emit ready(scaled);
}

void Interpretator::grabImage(QImage img)
{
    QMutexLocker locker(&mutex);
    old_image = img;
    new_image = QImage(2 * old_image.width(), old_image.height(), QImage::Format_RGB888);
}

void Interpretator::recover()
{
    QMutexLocker locker(&mutex);
    if (!isRunning())
    {
        start();    // starts just 1 time
    }
}

void Interpretator::setStandartCoefficients()
{
    QMutexLocker locker(&mutex);
    Kr = 0.299;
    Kb = 0.114;
}

void Interpretator::setHDCoefficients()
{
    QMutexLocker locker(&mutex);
    Kr = 0.2126;
    Kb = 0.0722;
}

void Interpretator::setGamma(float new_gamma)
{
    QMutexLocker locker(&mutex);
    gamma = new_gamma;
}

void Interpretator::setScale(int new_scale)
{
    QMutexLocker locker(&mutex);
    scale = new_scale;
}

//returns value [0..1] - for luma, [-0.5 .. 0.5] - for chroma
float Interpretator::normalize(bool isLumi, int component)
{
    return isLumi ? ((component - 16) / 219.0) : ((component - 16) / 224.0) - 0.5;
}

int Interpretator::checkRgbRanges(float value)
{
    value *= 255;
    if(0 > value)
    {
        return 0;
    }
    if(255 < value)
    {
        return 255;
    }
    return static_cast<int>(value + 0.5);
}

bool Interpretator::inRanges(int i, int j, int wdth, int hgth)
{
    return (0 <= i) && (0 <= j) && (wdth > i) && (hgth > j) ? true : false;
}
