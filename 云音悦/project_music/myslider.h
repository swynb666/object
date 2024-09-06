#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QObject>
#include <QSlider>
#include <QMouseEvent>
#include <QDebug>

class MySlider : public QSlider
{
public:
    MySlider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *ev) override;
    ~MySlider();
};

#endif // MYSLIDER_H
