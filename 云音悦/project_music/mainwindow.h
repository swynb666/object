#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QTextStream>
#include <QTcpSocket>
#include <QTcpServer>
#include <string.h>
#include "regwindow.h"
#include "userwindow.h"
#include "localmusic.h"
#include "message.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UserWindow;
class RegWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class UserWindow;
    friend class MyFavorite;
    friend class LocalMusic;
    friend class RegWindow;
public:
    MainWindow(QWidget *parent = nullptr);
    virtual void paintEvent(QPaintEvent *event) override; //重绘,
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    bool m_leftMousePressed;
    QPoint m_StartPoint;
    ~MainWindow();
private slots:
    void onSend();
    void onAccept();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    RegWindow *regWindow;
    UserWindow *userWindow;
    MyFavorite *myFavorite;
    Ui::MainWindow *ui;
    QTcpSocket *client;
    REG reg;
    QMSG msg;
    QList<song> songList;
    //QList<song> song_List; // 存放播放列表歌曲名
    QList<int> checkList;
    QMediaPlaylist * playlist;
    QMediaPlaylist * myFaovorlist;
    QMediaPlayer * player;
};
#endif // MAINWINDOW_H
