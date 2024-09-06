#ifndef MYFAVORITE_H
#define MYFAVORITE_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QTextStream>
#include <QTcpSocket>
#include <QTcpServer>
#include <string.h>
#include <QList>
#include <QListWidgetItem>

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
#include <QThread>
#include <QEventLoop>
#include "message.h"
#include "customdelegete.h"

class MainWindow;

namespace Ui {
class MyFavorite;
}

class MyFavorite : public QWidget
{
    Q_OBJECT

public:
    explicit MyFavorite(MainWindow  *mainWindow, QWidget *parent = nullptr);
    ~MyFavorite();
    void showFavoriteWindow();
    void getMyfavorite();
public slots:


private slots:


    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::MyFavorite *ui;
    MainWindow *mainWindow;
    QTcpSocket *client;
    QStandardItemModel * pModel;
    QStringList titles;
    QMediaPlaylist * playlist;
    QMediaPlaylist * myFaovorlist;
    QMediaPlayer * player;

};

#endif // MYFAVORITE_H
