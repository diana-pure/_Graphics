#ifndef INTERPRETATOR_H
#define INTERPRETATOR_H

#include <QThread>
#include <QMutex>
#include <QImage>

class Interpretator : public QThread
{
    Q_OBJECT
public:
    explicit Interpretator(QObject *parent = 0);
    ~Interpretator();
    void recover();
    void setGamma(float);
    void setHDCoefficients();
    void setStandartCoefficients();

signals:
    void ready(QImage);

public slots:
    void setScale(int);
    void grabImage(QImage);

protected:
    void run();
    float normalize(bool, int);
    int checkRgbRanges(float);
    bool inRanges(int, int, int, int);

private:
    QImage old_image;
    QImage new_image;
    QMutex mutex;
    float Kr, Kb, gamma;
    int scale;
};

#endif // INTERPRETATOR_H
