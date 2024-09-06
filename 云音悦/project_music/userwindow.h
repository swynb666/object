#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QTextStream>
#include <QTcpSocket>
#include <QTcpServer>
#include <string.h>
#include <QList>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QTcpServer>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTime>
#include <QTimer>
#include <QMediaContent>

#include "message.h"
#include "customdelegete.h"
#include "myfavorite.h"
#include "localmusic.h"
class MainWindow;
class LocalMusic;
namespace Ui {
class UserWindow;
}



class UserWindow : public QMainWindow
{
    Q_OBJECT
    friend class MainWindow;
friend class MyFavorite;
friend class LocalMusic;
public:
    explicit UserWindow(MainWindow *mainwindow,QWidget *parent = nullptr);
    ~UserWindow();
    void paresJson(QByteArray jsonByte);
    QString millisecondsToTimeString(int milliseconds);
    virtual void paintEvent(QPaintEvent *event) override; //重绘,
    virtual void mousePressEvent(QMouseEvent *event) override; //鼠标点击标题栏移动
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    void showUserWindow(); // 重写show函数
    void playMyFavorite(int musicId, QString musicName);

    bool m_leftMousePressed;
    QPoint m_StartPoint;
public slots:
    void replyFinished(QNetworkReply *reply);
    void slot_positionChanged(qint64 position);
    void slot_durationChanged(qint64 duration);
private slots:
    void on_tableView_clicked(const QModelIndex &index); // 收藏
    void on_pushButton_search_clicked(); // 搜索按钮
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_pushButton_star_clicked(); // 开始暂停按钮
    void on_pushButton_priv_clicked(); // 上一首
    void on_horizontalSlider_sliderReleased(); //进度拖动
    void on_pushButton_2_clicked();
    void on_horizontalSlider_actionTriggered(int action);
    void on_pushButton_next_clicked(); // 下一首
    void on_horizontalSlider_volume_actionTriggered(int action);
    void on_horizontalSlider_volume_sliderReleased();

    void on_pushButton_minisize_clicked();

    void on_pushButton_closewin_clicked();

    void on_listWidget_2_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::UserWindow *ui;
    QNetworkAccessManager *manager;
    QStandardItemModel * pModel;
    QStringList titles;
    QMediaPlaylist * playlist;
    QMediaPlayer * player;
    MyFavorite *pMyfavor;
    LocalMusic *pLocal;
    QTcpSocket *client;
    REG reg;
   MainWindow *mainwindow;

};

#endif // USERWINDOW_H
