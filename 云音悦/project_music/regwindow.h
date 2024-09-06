#ifndef REGWINDOW_H
#define REGWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QTextStream>
#include <QTcpSocket>
#include <QTcpServer>
#include <string.h>
#include <QMessageBox>
#include "message.h"
#include "mainwindow.h"
namespace Ui {
class RegWindow;
}

class MainWindow;
class RegWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegWindow(MainWindow *mainwindow, QWidget *parent = nullptr);
    ~RegWindow();
    virtual void paintEvent(QPaintEvent *event) override; //重绘,
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    bool m_leftMousePressed;
    QPoint m_StartPoint;

private slots:
    void on_pushButton_miniSizeWin_clicked();
    void on_pushButton_closeWin_clicked();

    void on_pushButton_Ensure_clicked();

private:
    Ui::RegWindow *ui;
    QTcpSocket *client;
    MainWindow *mainwindow;
    QMessageBox msgBox;
};

#endif // REGWINDOW_H
