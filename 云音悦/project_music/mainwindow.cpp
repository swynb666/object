#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    setAttribute(Qt::WA_TranslucentBackground);//设置窗口透明化
    this->setWindowFlag(Qt::FramelessWindowHint);
    //设置主窗口风格
    this->setStyleSheet(
                "QMainWindow {"
                "border-radius: 10px;"                  /* 四角圆角的直径 */
                "border: none;"
                "background-image: url(:/登录页背景.png);" /* 背景图片 */
                "background-repeat: no-repeat;"          /* 不重复 */
                "background-position: center;"           /* 居中 */
                "}"
                );
    //设置头像label为圆形且不覆盖边框
    ui->label->setStyleSheet("border-image: url(:/听音乐.png);"
                             "border:3px solid black;"
                             "border-radius: 64px;"
                             );
    //缩小窗口按钮样式
    ui->pushButton->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/Minimiz-1.png);"
                "}"
                "QPushButton:hover {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/Minimize-2.png);"
                "}"
                "QPushButton:pressed {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/Minimize-2.png);"
                "}"
                );

    //关闭窗口按钮样式
    ui->pushButton_2->setStyleSheet(
                "QPushButton {"
                "image: url(:/关闭窗口.png);"
                "background-repeat: no-repeat;"
                "background-position: center;"
                "border-top-left-radius: 0px;"
                "border-bottom-left-radius: 0px;"
                "border-top-right-radius: 10px;"
                "border-bottom-right-radius: 0px;"
                "background-color: transparent;" // 设置背景透明
                "}"
                "QPushButton:hover {"
                "color: rgb(255, 255, 255);"
                "background-color: red;"
                "}"
                );
    //标题栏样式
    ui->widget->setStyleSheet(
                "QWidget {"
                "border-radius: 10px;"
                "border-bottom-left-radius: 0px;"
                "border-bottom-right-radius: 0px;"
                "}"
                );
    //设置输入账号LineEdit
    ui->lineEdit->setStyleSheet(
                "QLineEdit {"
                "color: rgba(255, 255, 255);"
                "border-radius: 10px;"          // 边框圆角
                "padding: 5px;"                 // 内边距
                "background-color: rgba(255, 255, 255, 128);" // 背景颜色（白色，半透明）
                "}"
                );
    //设置输入密码LineEdit
    ui->lineEdit_2->setStyleSheet(
                "QLineEdit {"
                "color: rgba(255, 255, 255);"
                "border-radius: 10px;"          // 边框圆角
                "padding: 5px;"                 // 内边距
                "background-color: rgba(255, 255, 255, 128);" // 背景颜色（白色，半透明）
                "}"
                );

    //设置自动登录样式
    ui->checkBox->setStyleSheet(
                "QCheckBox { "
                " border: 0px solid #FFFFFF;" /*设置qcheckbox控件的边框为0px（无边框）*/
                "border-radius: 20px;"
                "padding: 4px;"
                "spacing: 10px;"
                "}"
                );
    //设置记住密码样式
    ui->checkBox_2->setStyleSheet(
                "QCheckBox { "
                " border: 0px solid #FFFFFF;" /*设置qcheckbox控件的边框为0px（无边框）*/
                "border-radius: 20px;"
                "padding: 4px;"
                "spacing: 10px;"
                "}"
                );


    //登录按钮样式
    ui->pushButton_3->setStyleSheet(
                "QPushButton {"
                "background-color: #130454;"
                "border-radius:10px;"
                "color: rgb(255, 255, 255);"
                "}"
                );

    //注册按钮样式
    ui->pushButton_4->setStyleSheet(
                "QPushButton {"
                "background-color: #130454;"
                "border-radius:10px;"
                "color: rgb(255, 255, 255);"
                "}"
                );
    //信息提示按钮样式
//    ui->label->setStyleSheet(
//                "color: #CFCDCE;"
//                "background-color: transparent;" // 设置背景透明
//                );

    client = new QTcpSocket(this);
    client->connectToHost("127.0.0.1", 9000);
    connect(client, &QTcpSocket::connected, this, &MainWindow::onSend);
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::onAccept);

    playlist = new QMediaPlaylist;
    myFaovorlist = new QMediaPlaylist;
    player = new QMediaPlayer;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
//需添加头文件 <QPainter>和 <QStyleOption>

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int x=ui->widget->x();
    int y=ui->widget->y();
    int w=ui->widget->width();
    int h=ui->widget->height();
    //只能是鼠标左键移动和改变大小
    if((event->button() == Qt::LeftButton) //处于左键状态
        &&(event->x()>x&&event->x()<x+w)  //x坐标位置判定
        &&(event->y()>y&&event->y()<y+h)) //y坐标位置判定
    {
    m_leftMousePressed = true;  //标志位置为真
    //按下时鼠标左键时，窗口在屏幕中的坐标
    m_StartPoint = event->globalPos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口,由于取消了标题栏，因此需要自己实现拖动窗口功能
    if (m_leftMousePressed)
    {
        QPoint curPoint = event->globalPos();   //按住移动时的位置
        QPoint movePoint = curPoint - m_StartPoint; //与初始坐标做差，得位移
        //普通窗口
        QPoint mainWinPos = this->pos();
        //设置窗口的全局坐标
        this->move(mainWinPos.x() + movePoint.x(), mainWinPos.y() + movePoint.y());
        m_StartPoint = curPoint;
     }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_leftMousePressed = false;//释放鼠标，标志位置为假
}


void MainWindow::onSend()
{
    qDebug()<<"MainWindow连接服务器成功"<<endl;
}

/*
数据粘包问题
解决方法：
    发送发和接收方都加入一个数据长度，以便接收方能够读取指定长度知道读完这个数据
*/
void MainWindow::onAccept()
{
    client = qobject_cast<QTcpSocket *>(sender());
    qDebug()<<"client = "<<client<<endl;
    if(client)
    {
        while(client->bytesAvailable())
        {
            int total = 0;
            client->read((char *)&total, sizeof(int));
            qDebug()<<"获取服务器消息大小:"<<total;
            QByteArray data;
            int readNum = total;
            //把数据全部读完
            while (true)
            {

                QByteArray tmp = client->read(readNum);
                readNum -= tmp.length();
                data.append(tmp);
                if(readNum>0)
                {
                    qDebug()<<"正在读取数据:"<<readNum;
                    client->waitForReadyRead();
                }

                else
                {
                    memcpy(&msg, data.data(), sizeof(msg));
                    qDebug()<<"读取完成,数据类型:"<<msg.type;
                    switch (msg.type)
                    {
                    case -1: // 登录失败信号
                    {
                        qDebug()<<"登录失败"<<endl;
                        // 创建一个消息框
                        QMessageBox msgBox;
                        msgBox.setWindowTitle("LoginError");
                        msgBox.setText("账号或密码错误");
                        msgBox.exec();
                        ui->lineEdit->clear();
                        ui->lineEdit_2->clear();
                        break;
                    }

                    case 0: // 登录成功信号
                    {
                        qDebug()<<"登录成功"<<endl;
                        this->hide();
                        userWindow = new UserWindow(this);
                        userWindow->showUserWindow();

                        break;
                    }
                    case 1: // userWindow信息
                    {
                        qDebug()<<"收到发给userWindow的信息"<<endl;
                        break;
                    }
                    case 2: // myfavorite信息
                    {
                        qDebug()<<"收到发给myfavorite的信息";
                        song *Song;
                        Song = (song *)msg.buf;
                        songList.push_back(*Song);

                        if(!checkList.contains(Song->musicId))
                        {
                            checkList.push_back(Song->musicId);

                            QString play_url = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(Song->musicId);
                            playlist->addMedia(QUrl(play_url));//把歌曲加入到播放列表
//                            song_List.push_back(*Song);
                            qDebug()<<"将id为"<<Song->musicId<<"的歌加入链表";
                        }
                        qDebug()<<Song->songName<<" "<<Song->singerName<<endl;
                        break;
                    }
                    case -2:
                    {
                        qDebug()<<"给myfavorite发送信息完毕"<<endl;
                        userWindow->pMyfavor->getMyfavorite();
                        songList.clear();
                        break;
                    }
                    case 3: // 注册成功信号
                    {
                        qDebug()<<"注册成功";
                        break;
                    }
                    case -3: // 注册失败信号
                    {
                        qDebug()<<"注册失败";
                        // 创建一个消息框
                        break;
                    }
                    default:
                        qDebug()<<"没有该消息类型"<<endl;
                    }
                    break;
                }

            }

        }



    }
}

void MainWindow::on_pushButton_clicked()
{
    this->showMinimized();
}

void MainWindow::on_pushButton_4_clicked() // 注册槽函数
{
    regWindow = new RegWindow(this);
    regWindow->show();
    //this->hide();
}

void MainWindow::on_pushButton_3_clicked() // 登录按钮槽函数
{
    QMSG msg;
    strcpy(reg.name, ui->lineEdit->text().toStdString().c_str());
    strcpy(reg.passwd, ui->lineEdit_2->text().toStdString().c_str());
    memcpy(msg.buf, &reg, sizeof(reg));
    msg.type = 2; // 设置类型2表示登录请求
    client->write((char *)&msg, sizeof(msg));
//    userWindow->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    this->close();
}
