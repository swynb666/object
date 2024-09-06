#ifndef LOCALMUSIC_H
#define LOCALMUSIC_H

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
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include "message.h"
#include "customdelegete.h"
#include "mainwindow.h"

namespace Ui {
class LocalMusic;
}

class LocalMusic : public QWidget
{
    Q_OBJECT

public:
    explicit LocalMusic(MainWindow *mainWindow ,QWidget *parent = nullptr);
    ~LocalMusic();
    void showLocalMusic();
private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::LocalMusic *ui;
    QStandardItemModel *pModel;
    QMediaPlaylist *plocalList;
    QMediaPlayer *player;
    QStringList titles;
    MainWindow *mainwindow;
};

#endif // LOCALMUSIC_H
