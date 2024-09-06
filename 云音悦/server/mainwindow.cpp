#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化server
    server = new QTcpServer(this);

    //设置端口
    server->listen(QHostAddress::Any, 9000);

    //连接槽函数
    connect(server, &QTcpServer::newConnection, this, &MainWindow::onConnect);

    //初始化数据库
    sqliteOP = new sqliteoperator; // 创建数据连接
    sqliteOP->openDb();

    //创建一个用户数据表存放用户数据信息
    //    sqliteOP->normalExec("create table user (name varchar(128), passwd varchar(128), sex int)");
    //    sqliteOP->closeDb();

    //QjsonDucment
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAccept()
{
    client = qobject_cast<QTcpSocket *>(sender());
    if(client)
    {
        QMSG msg;
        client->read((char *)&msg, sizeof(QMSG));

        switch (msg.type)
        {
        case 1: // 注册请求
        {
            ui->textEdit->append("注册请求");
            REG *reg = (REG *)msg.buf;
            qDebug()<<"账号:"<<reg->name<<"密码:"<<reg->passwd<<"性别:"<<reg->sex;
            QSqlQuery sqlQuery; // 存放查询结果
            QString sql = QString("select * from user where name='%1' and passwd='%2'").arg(reg->name).arg(reg->passwd);
            sqliteOP->normalQuery(sqlQuery, sql);
            if(sqlQuery.next())
            {
                qDebug()<<"用户已存在";
                ui->textEdit->append("注册失败，用户已存在");

                msg.type = -3; // 返回信号0

                int len = sizeof(msg);
                client->write((char *)&len, sizeof(len));
                client->write((char *)&msg, sizeof(msg));
            }
            else
            {
                QString sql = QString("insert into user values('%1','%2', %3)").arg(reg->name).arg(reg->passwd).arg(reg->sex);
                sqliteOP->normalExec(sql);
                ui->textEdit->append("注册成功");
                //注册成功创建一个独属于用户喜欢的歌单表
                sql.clear();
                sql = QString("create table %1 (songName varchar(128), singerName varchar(128), albumName varchar(128), duration2 int, musicId int)").arg(reg->name);
                sqliteOP->normalExec(sql);

                ui->textEdit->append("创建一个独属于用户喜欢的歌单表");
                msg.type = 3; // 返回成功信号0

                int len = sizeof(msg);
                client->write((char *)&len, sizeof(len));
                client->write((char *)&msg, sizeof(msg));
            }
            break;

        }

        case 2: // 登录请求
        {
            ui->textEdit->append("登录请求");
            REG *reg = (REG *)msg.buf;
            qDebug()<<"账号:"<<reg->name<<"密码:"<<reg->passwd;
            QSqlQuery sqlQuery; // 存放查询结果
            QString sql = QString("select * from user where name='%1' and passwd='%2'").arg(reg->name).arg(reg->passwd);
            sqliteOP->normalQuery(sqlQuery, sql);
            if(sqlQuery.next())
            {
                ui->textEdit->append("登录成功");
                userName.push_back(reg->name);
                qDebug()<<"用户登录成功"<<sqlQuery.value(0)<<sqlQuery.value(1)<<sqlQuery.value(3)<<endl;
                msg.type = 0; // 返回成功信号0
                int len = sizeof(msg);
                client->write((char *)&len, sizeof(len));
                client->write((char *)&msg, sizeof(msg));

            }
            else
            {
                ui->textEdit->append("登录失败");
                qDebug()<<"登录失败";
                msg.type = -1; // 返回失败信号-1
                int len = sizeof(msg);
                client->write((char *)&len, sizeof(len));
                client->write((char *)&msg, sizeof(msg));
            }
            break;
        }
        case 3: // 请求服务器将用户的喜欢歌单信息发送给我
        {
            REG *reg = (REG *)msg.buf;
            QString nowUserName = reg->name;
            qDebug()<<"用户"<<nowUserName<<"请求歌单信息"<<msg.type<<endl;
            QSqlQuery sqlQuery; // 存放查询结果
            QString sql = QString("select * from %1").arg(nowUserName);
            sqliteOP->normalQuery(sqlQuery, sql);
            //先将所有歌的信息存入链表中
            ui->textEdit->append("请求喜欢的歌曲");

            while (sqlQuery.next())
            {
                song Song;

                QString data = sqlQuery.value(0).toString();
                strcpy(Song.songName, data.toStdString().c_str());

                data = sqlQuery.value(1).toString();
                strcpy(Song.singerName, data.toStdString().c_str());

                data = sqlQuery.value(2).toString();
                strcpy(Song.albumName, data.toStdString().c_str());

                Song.duration2 = sqlQuery.value(3).toInt();
                Song.musicId = sqlQuery.value(4).toInt();
                qDebug() <<nowUserName <<"Song 信息: " << Song.songName << ", " << Song.singerName<<", "<<Song.albumName<<", "<<Song.musicId<<endl;
                msg.type = 2; // 返回喜欢的歌单信号2
                memcpy(msg.buf, &Song, sizeof (Song));
                int len = sizeof(msg);
                client->write((char *)&len, sizeof(len));

                client->write((char *)&msg, sizeof(msg));
                client->waitForBytesWritten();
                ui->textEdit->append(Song.songName);
                qDebug()<<"len = "<<len<<" "<<client<< endl;
            }
            //再将链表发送给客户端
            msg.type = -2;
            int len = sizeof(msg);
            client->write((char *)&len, sizeof(len));
            qDebug()<<"给客户端发送-2表示结束"<<" "<<client;
            client->write((char *)&msg, sizeof(msg));

            break;
        }
        case 4: // 客户端请求取消收藏
        {
            favoriteSong *fSong;
            fSong = (favoriteSong *)msg.buf;
            ui->textEdit->append("取消收藏了一首歌");
            QString sql = QString("delete from '%1' where songName='%2' and singerName='%3' and albumName='%4' and musicId=%5").arg(fSong->userName).arg(fSong->Song.songName).arg(fSong->Song.singerName).arg(fSong->Song.albumName).arg(fSong->Song.musicId);
            sqliteOP->normalExec(sql);
            qDebug()<<fSong->userName<<" "<<fSong->Song.songName<<endl;
            break;
        }
        case 5:
        {
            favoriteSong *fSong;
            fSong = (favoriteSong *)msg.buf;
            ui->textEdit->append("收藏了一首歌");
            QString sql = QString("insert into '%1' values('%2','%3', '%4', %5, %6)").arg(fSong->userName).arg(fSong->Song.songName).arg(fSong->Song.singerName).arg(fSong->Song.albumName).arg(fSong->Song.duration2).arg(fSong->Song.musicId);
            sqliteOP->normalExec(sql);
            qDebug()<<fSong->userName<<" "<<fSong->Song.songName<<endl;
            break;
        }

        }

    }
}

void MainWindow::onConnect()
{
    client = server->nextPendingConnection();
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::onAccept);
    //如果客户端断开连接，也会收到断开连接的信号，这里关联一个专门处理客户端断开连接的槽函数
    connect(client, &QTcpSocket::disconnected, this, &MainWindow::onDisConnect);
}

void MainWindow::onDisConnect()
{
    qDebug()<<"客户端断开连接"<<endl;
}

void MainWindow::on_pushButton_clicked()
{
    if(client)
    {
        QString data;
        data = ui->lineEdit->text();
        std::string mes = data.toStdString();
        client->write(mes.c_str());
    }
}
