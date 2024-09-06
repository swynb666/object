#include "userwindow.h"
#include "ui_userwindow.h"
#include"mainwindow.h"
UserWindow::UserWindow(MainWindow *mainwindow,QWidget *parent) :
    QMainWindow(parent),mainwindow(mainwindow),
    ui(new Ui::UserWindow)
{
    ui->setupUi(this);


    setAttribute(Qt::WA_TranslucentBackground);//设置窗口透明化
    this->setWindowFlag(Qt::FramelessWindowHint);
    //连接到服务器
    client = mainwindow->client;


    //添加左侧listwidget项
    // 添加项目并设置样式和图标
       QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/item_桃心.png"), "我喜欢的音乐");
       ui->listWidget_2->addItem(item1);

       QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/item_本地.png"), "本地音乐");
       ui->listWidget_2->addItem(item2);

    qDebug()<<"userWin当前："<<mainwindow->reg.name<<endl;

    pLocal = new LocalMusic(this->mainwindow,ui->tableView);
    pMyfavor = new MyFavorite(this->mainwindow, ui->tableView); // 初始化我的喜欢
    pMyfavor->showFavoriteWindow();




    //设置主窗口风格
    this->setStyleSheet(
                "UserWindow {"
                "background-color: #2B2B2B;" /* 背景颜色（灰色） */
                "border-radius: 10px;"                  /* 四角圆角的直径 */
                "border: none;"
                "background-repeat: no-repeat;"          /* 不重复 */
                "background-position: center;"           /* 居中 */
                "}"
                );

    //缩小窗口按钮样式
    ui->pushButton_minisize->setStyleSheet(
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
    ui->pushButton_closewin->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/关闭窗口.png);"
                "}"
                "QPushButton:hover {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/关闭窗口2.png);"
                "}"
                "QPushButton:pressed {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/关闭窗口2.png);"
                "}"
                );
    //搜索框样式
    ui->lineEdit_search->setStyleSheet(
                "QLineEdit {"
                "    border: none; " // 设置边框
                "    background-color: #2A2A2A;"
                "    border-top-right-radius: 10px;"
                "    border-bottom-right-radius: 10px;"
                "    color: #D7DBDA;"
                "    padding: 2px;" // 设置内边距
                "}"

                );
    //搜索按钮样式
    ui->pushButton_search->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    border-top-left-radius: 10px;"
                "    border-bottom-left-radius: 10px;"
                "    background-color: #2A2A2A;"
                "    image: url(:/搜索1.png);"
                "}"
                "QPushButton:hover {"
                "    border: none;"
                "    border-top-left-radius: 10px;"
                "    border-bottom-left-radius: 10px;"
                "    background-color: #2A2A2A;"
                "    image: url(:/搜索2.png);"
                "}"
                "QPushButton:pressed {"
                "    border: none;"
                "    border-top-left-radius: 10px;"
                "    border-bottom-left-radius: 10px;"
                "    background-color: #2A2A2A;"
                "    image: url(:/搜索2.png);"
                "}"
                );
    //播放和暂停按钮样式
    ui->pushButton_star->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/播放.png);"
                "}"
                );
    //上一首按钮
    ui->pushButton_priv->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/上一首.png);"
                "}"
                );
    //下一首按钮
    ui->pushButton_next->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/下一首.png);"
                "}"
                );


    //标题栏样式
    ui->widget->setStyleSheet(
                "QWidget#widget {"
                "border: none;"
                "border-bottom: 2px solid #79313F;"
                "background-color: #222224;"
                "}"
                );

    //左侧widget_5样式
    ui->widget_5->setStyleSheet(
                "QWidget#widget_5 {"
                "border: none;"
                "border-right: 1px solid #3E3E3E;"
                "background-color: #2B2B2B;"
                "}"
                );
    //底部widget_3样式
    ui->widget_3->setStyleSheet(
                "QWidget#widget_3 {"
                "border: none;"
                "background-color: #222224;"
                "}"
                );
    //中间widget_2样式
    ui->widget_2->setStyleSheet(
                "QWidget#widget_2 {"
                "background-color: #2B2B2B;"
                "}"
                );

    //中间tableview样式设置
    ui->tableView->setAlternatingRowColors(true); // 设置奇偶行颜色
    ui->tableView->setStyleSheet(
        "QTableView {"
        "    background-color: #2B2B2B;" // 设置背景颜色
        "    color: #B5B5B5;" // 设置字体颜色
        "    alternate-background-color: #414141;" // 设置奇偶行背景颜色
        "    border: 1px solid #2B2B2B;" // 设置边框颜色和宽度
        "}"
        "QTableView::item:hover {"
        "    background-color: #666666;" // 鼠标悬停时行的背景颜色
        "}"
        "QTableView::item:selected {"
            "    background-color: transparent;" // 取消选中项的背景颜色
            "    color: #B5B5B5;" // 保持选中项的字体颜色
            "    outline: none;" // 取消焦点矩形
            "}"
    );
    // 关闭单元格的选中效果
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);

    // 关闭表头的鼠标点击功能
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView->verticalHeader()->setSectionsClickable(false);


    //左侧listWidget_2样式设置
    ui->listWidget_2->setStyleSheet(
        "QListWidget {"
        "    background-color: transparent;" // 设置背景透明
        "    color: #D7DBDA;" // 设置字体颜色
        "    border-right: 2px solid #3E3E3E;" // 设置右边框颜色和宽度
        "    border-left: 1px solid #2B2B2B;" // 设置左边框颜色和宽度
        "    border-top: 1px solid #2B2B2B;" // 设置上边框颜色和宽度
        "    border-bottom: 1px solid #2B2B2B;" // 设置下边框颜色和宽度
        "    font-size: 16pt;" /* 设置字符大小 */
        "    icon-size: 32px 32px;"
        "}"
        "QListWidget::item {"
        "    padding: 5px;"
        "}"
    );


    //播放进度条样式
    ui->horizontalSlider->setStyleSheet(
                "QSlider#horizontalSlider {"
                "    border-radius: 5px;" // 设置边框圆角
                "}"
                "QSlider::groove:horizontal {"
                "    background: #CDCDCF;" // 设置未经过的进度颜色
                "    height: 4px;" // 设置滑槽高度
                "    border-radius: 2px;" // 设置滑槽圆角
                "}"
                "QSlider::handle:horizontal {"
                "    background: #CE5961;" // 设置滑块颜色
                "    width: 10px;" // 设置滑块宽度
                "    height: 10px;" // 设置滑块高度
                "    margin: -5px 0;" // 调整滑块位置
                "    border-radius: 5px;" // 设置滑块圆角
                "}"
                "QSlider::add-page:horizontal {"
                "    background: #CDCDCF;" // 设置已经经过的进度颜色
                "    border-radius: 2px;" // 设置进度圆角
                "}"
                "QSlider::sub-page:horizontal {"
                "    background: #CE5961;" // 设置未经过的进度颜色
                "    border-radius: 2px;" // 设置进度圆角
                    "}"
                );
    // 设置歌曲进行时间 label 样式
    ui->label_start_time->setStyleSheet(
        "QLabel {"
        "    color: #CDCDCF;" // 设置字体颜色
        "    background-color: transparent;" // 设置背景透明
        "}"
    );

    // 设置歌曲总时间 label 样式
    ui->label_en_time->setStyleSheet(
        "QLabel {"
        "    color: #CDCDCF;" // 设置字体颜色
        "    background-color: transparent;" // 设置背景透明
        "}"
    );

    //音量进度条样式
    ui->horizontalSlider_volume->setStyleSheet(
                "QSlider#horizontalSlider_volume {"
                    "    border-radius: 5px;" // 设置边框圆角
                    "}"
                    "QSlider::groove:horizontal {"
                    "    background: #CDCDCF;" // 设置未经过的进度颜色
                    "    height: 4px;" // 设置滑槽高度
                    "    border-radius: 2px;" // 设置滑槽圆角
                    "}"
                    "QSlider::handle:horizontal {"
                    "    background: #CE5961;" // 设置滑块颜色
                    "    width: 10px;" // 设置滑块宽度
                    "    height: 10px;" // 设置滑块高度
                    "    margin: -5px 0;" // 调整滑块位置
                    "    border-radius: 5px;" // 设置滑块圆角
                    "}"
                    "QSlider::add-page:horizontal {"
                    "    background: #CDCDCF;" // 设置已经经过的进度颜色
                    "    border-radius: 2px;" // 设置进度圆角
                    "}"
                    "QSlider::sub-page:horizontal {"
                    "    background: #CE5961;" // 设置未经过的进度颜色
                    "    border-radius: 2px;" // 设置进度圆角
                    "}"
                );

    //音量键样式
    ui->pushButton_2->setStyleSheet(
                "QPushButton#pushButton_2 {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/音量.png);"
                "}"
                );
    //播放列表样式
    ui->pushButton_playlist->setStyleSheet(
                "QPushButton#pushButton_playlist {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/播放列表.png);"
                "}"
                "QPushButton#pushButton_playlist:hover {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/播放列表2.png);"
                "}"
                "QPushButton#pushButton_playlist:pressed {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/播放列表2.png);"
                "}"
                );

    //设置封面label为圆形且不覆盖边框
    ui->label_cover->setStyleSheet(
                "border-image: url(:/音乐.png);"
                "    background-color: transparent;" // 设置背景透明
                );

    // 设置当前歌曲名 label 样式
    ui->label_songname->setStyleSheet(
        "QLabel {"
        "    background-color: transparent;" // 设置背景透明
        "    color: #C6C6C6;" // 设置字体颜色
        "}"
    );

    //设置标题栏lable
    ui->label->setStyleSheet(
                "border-image: url(:/音符.png);"
                "background-color: transparent;" // 设置背景透明
                );

    //设置标题栏文字lable
    ui->label_2->setStyleSheet(
                "background-color: transparent;" // 设置背景透明
                "color: #CCCACB;"
                );
    ui->label_2->setText("云音悦");

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(replyFinished(QNetworkReply *)));

    pModel = new QStandardItemModel(ui->tableView); // 实例化模型，并指定模型控件
    ui->tableView->setModel(pModel);
    ui->tableView->setItemDelegateForColumn(4,new customDelegete);
    titles << "歌名" << "歌手" << "专辑" << "时长" << "收藏" <<"Id";

    //初始化歌曲链表
    player = mainwindow->player;
    playlist = mainwindow->playlist;
    // 设置播放列表的循环模式为循环播放
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(mainwindow->playlist);
    //设置音乐调节范围
    ui->horizontalSlider_volume->setRange(0, 100);
    ui->horizontalSlider_volume->setValue(50);

    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(slot_positionChanged(qint64)));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(slot_durationChanged(qint64)));
}

UserWindow::~UserWindow()
{
    delete ui;
}

void UserWindow::paresJson(QByteArray jsonByte)
{
    QString songname_original2; //歌曲名
    QString singername2;        //歌手
    QString album_name2;        //专辑
    int duration2;          	//时间
    int music_id;               //音乐id

    QJsonParseError json_error;//JSON解析错误对象
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonByte, &json_error);

    if (json_error.error == QJsonParseError::NoError)//判断解析过程是否有错误
    {
        if (parse_doucment.isObject())//判断文档是否是一个json对象
        {
            QJsonObject rootObj = parse_doucment.object();//获取文档JSON对象
            if (rootObj.contains("result"))//对象中是否包含键result
            {
                QJsonValue valuedata = rootObj.value("result");//获得result对应的值
                if (valuedata.isObject())//result对应的值是否是一个对象
                {
                    QJsonObject valuedataObject = valuedata.toObject();//把QJsonValue转为JsonObject对象
                    if (valuedataObject.contains("songs"))
                    {
                        QJsonValue valueArray = valuedataObject.value("songs");
                        if (valueArray.isArray())
                        {
                            //把结果转为QJsonArray数组才能遍历
                            QJsonArray array = valueArray.toArray();
                            int size = array.size();//(size>36?36:size)
                            for (int i = 0; i <size; i++)
                            {
                                QJsonValue value = array.at(i);
                                if (value.isObject())
                                {
                                    //把数组的第i个元素变成一个JSON对象
                                    QJsonObject object = value.toObject();
                                    if (object.contains("album"))//专辑1
                                    {
                                        QJsonValue album_Object_1 = object.value("album");
                                        if (album_Object_1.isObject())
                                        {
                                            QJsonObject album_Object_1_tmp=album_Object_1.toObject();
                                            if(album_Object_1_tmp.contains("name"))
                                            {
                                                QJsonValue ID_value = album_Object_1_tmp.value("name");
                                                if (ID_value.isString())
                                                {
                                                    album_name2 = ID_value.toString();
                                                }
                                            }
                                        }
                                    }

                                    if (object.contains("artists"))//歌手
                                    {
                                        QJsonValue artists_value_tmp = object.value("artists");
                                        if(artists_value_tmp.isArray())
                                        {
                                            QJsonArray artists_array = artists_value_tmp.toArray();
                                            int artists_size=artists_array.size();
                                            for(int k=0;k<artists_size;k++)
                                            {
                                                QJsonValue artists_name=artists_array.at(k);
                                                if(artists_name.isObject())
                                                {
                                                    QJsonObject artists_name_object=artists_name.toObject();
                                                    if(artists_name_object.contains("name"))
                                                    {
                                                        QJsonValue artistsname = artists_name_object.take("name");
                                                        if (artistsname.isString())
                                                        {
                                                            if(k==0)
                                                            {
                                                                singername2 = artistsname.toString();
                                                            }
                                                            else
                                                            {
                                                                singername2 = singername2 + "/" +artistsname.toString();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if (object.contains("name"))//歌曲名
                                    {
                                        QJsonValue Album_ID_value = object.take("name");
                                        if (Album_ID_value.isString())
                                        {
                                            songname_original2 = Album_ID_value.toString();
                                        }
                                    }
                                    if (object.contains("duration"))//时长
                                    {
                                        QJsonValue AlbumID_value = object.take("duration").toInt();
                                        duration2 = AlbumID_value.toInt();
                                        duration2=duration2/1000;
                                    }
                                    if (object.contains("id"))//歌曲id
                                    {
                                        QJsonValue FileHash_value = object.take("id");
                                        if (!FileHash_value.isNull())
                                        {
                                            //用Vector保存每首歌曲的album_id
                                            music_id=FileHash_value.toInt();
                                        }
                                    }
                                    song _song;
                                    strcpy(_song.songName, songname_original2.toStdString().c_str());
                                    strcpy(_song.singerName, singername2.toStdString().c_str());
                                    strcpy(_song.albumName, album_name2.toStdString().c_str());
                                    _song.duration2 = duration2;
                                    _song.musicId = music_id;



                                    pModel->setHorizontalHeaderLabels(titles);
                                    QList<QStandardItem *> sti;
                                    if(mainwindow->checkList.contains(_song.musicId))
                                    {
                                        qDebug()<<"已收藏"<<_song.songName;
                                        sti<< new QStandardItem(songname_original2)<< new QStandardItem(singername2)<< new QStandardItem(album_name2)<<new QStandardItem(QString().number(duration2))<<new QStandardItem("已收藏")<<new QStandardItem(QString().number(music_id));
                                    }

                                    else
                                    {
                                        qDebug()<<"未收藏"<<_song.songName<<" "<<_song.musicId;
                                        sti<< new QStandardItem(songname_original2)<< new QStandardItem(singername2)<< new QStandardItem(album_name2)<<new QStandardItem(QString().number(duration2))<<new QStandardItem("未收藏")<<new QStandardItem(QString().number(music_id));
                                    }


                                    // 设置每个 item 的对齐方式为居中
                                    for (QStandardItem *item : sti) {
                                        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                                    }
                                    ui->tableView->hideColumn(ui->tableView->model()->columnCount() - 1);
                                    pModel->appendRow(sti);


                                    // 设置每一列的宽
                                    ui->tableView->setColumnWidth(0, 256);
                                    ui->tableView->setColumnWidth(1, 128);
                                    ui->tableView->setColumnWidth(2, 256);
                                    ui->tableView->setColumnWidth(3, 256);
                                    ui->tableView->setColumnWidth(4, 128);


                                    // 设置每一行的高度
                                    for (int row = 0; row < pModel->rowCount(); ++row) {
                                        ui->tableView->setRowHeight(row, 50); // 设置每一行的高度为30像素
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << json_error.errorString()<<"111";
    }
}

QString UserWindow::millisecondsToTimeString(int milliseconds)
{

    // 使用 QTime 类来处理时间格式
    QTime time = QTime::fromMSecsSinceStartOfDay(milliseconds);
    return time.toString("mm:ss");

}

void UserWindow::replyFinished(QNetworkReply *reply)
{
    qDebug()<<"网页响应已完成"<<endl;
    QVariant ret = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug()<<"网页返回状态为"<<ret<<endl;
    if(ret == 200) // 代表网页访问成功
    {
        QByteArray data = reply->readAll(); // 读取所有网络数据
        paresJson(data);
    }
}

void UserWindow::slot_positionChanged(qint64 position) // 设置歌曲的播放时间，以及滑块的位置改变
{
    ui->label_start_time->setText(millisecondsToTimeString(position));
    ui->horizontalSlider->setValue(position);
}

void UserWindow::slot_durationChanged(qint64 duration)
{
    ui->pushButton_star->setStyleSheet(
                "QPushButton {"
                "    border: none;"
                "    background-color: transparent;"
                "    image: url(:/暂停.png);"
                "}"
                );
    ui->label_en_time->setText(millisecondsToTimeString(duration)); // 设置歌曲总时间
    ui->horizontalSlider->setRange(0, duration);
}

void UserWindow::on_tableView_clicked(const QModelIndex &index)
{
    if(index.column() != 4)//如果不是第四列就不要改变
    {
        return;
    }
    QVariant ret = pModel->data(index);//获得索引标识的模型数据
    QStandardItem * item = pModel->item(index.row(),index.column());
    delete item;//释放掉原有的选项
    //    以下代码获得点击的行的歌曲信息，告诉服务器
    favoriteSong fSong;
    strcpy(fSong.userName, mainwindow->reg.name);

    int row = index.row();
    int column = index.column()-4;

    QModelIndex cellIndex = pModel->index(row, column);
    QVariant data = pModel->data(cellIndex);
    QString songName = data.toString();
    strcpy(fSong.Song.songName, songName.toStdString().c_str());

    cellIndex = pModel->index(row, column+1);
    data = pModel->data(cellIndex);
    QString singerName = data.toString();
    strcpy(fSong.Song.singerName, singerName.toStdString().c_str());

    cellIndex = pModel->index(row, column+2);
    data = pModel->data(cellIndex);
    QString albumName = data.toString();
    strcpy(fSong.Song.albumName, albumName.toStdString().c_str());

    cellIndex = pModel->index(row, column+3);
    data = pModel->data(cellIndex);
    fSong.Song.duration2 = data.toInt();

    cellIndex = pModel->index(row, column+5);
    data = pModel->data(cellIndex);
    fSong.Song.musicId = data.toInt();


    qDebug()<<fSong.userName<<" "<<fSong.Song.songName<<" "<<fSong.Song.singerName;
    if(ret == "未收藏")
    {
        pModel->setItem(index.row(),index.column(),new QStandardItem("已收藏"));
        QMSG msg;
        msg.type = 5;
        memcpy(msg.buf, &fSong, sizeof(favoriteSong));
        client->write((char *)&msg, sizeof (msg));
    }
    else if(ret == "已收藏")
    {
        pModel->setItem(index.row(),index.column(),new QStandardItem("未收藏"));
        QMSG msg;
        msg.type = 4;
        memcpy(msg.buf, &fSong, sizeof(favoriteSong));
        client->write((char *)&msg, sizeof (msg));
        mainwindow->myFaovorlist->removeMedia(mainwindow->checkList.indexOf(fSong.Song.musicId));
        mainwindow->checkList.removeOne(fSong.Song.musicId);

    }
}

void UserWindow::on_pushButton_search_clicked()
{
    pModel->removeRows(0, pModel->rowCount()); // 清空上一次搜索的内容
    QString data = ui->lineEdit_search->text();
    QString url = QString("http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s=%1&type=1&offset=0&total=true&limit=10").arg(data);
    manager->get(QNetworkRequest(QUrl(url)));
    pMyfavor->hide();
    pLocal->hide();
}

void UserWindow::on_tableView_doubleClicked(const QModelIndex &index) // 播放
{
    //构建歌曲播放链接
    QModelIndex cellIndex = pModel->index(index.row(),index.column());
    QVariant data = pModel->data(cellIndex);
    cellIndex = pModel->index(index.row(), index.column()+5);
    data = pModel->data(cellIndex);
    int musicId = data.toInt();
    QString play_url = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(musicId);

    // 插入歌曲到当前播放的后一个位置
    player->setPlaylist(playlist);
    playlist->insertMedia(playlist->currentIndex()+1, QUrl(play_url));
    playlist->setCurrentIndex(playlist->currentIndex()+1);
    player->play();

//    song Song;
    cellIndex = pModel->index(index.row(), index.column()+0);
    data = pModel->data(cellIndex);
    QString musicName = data.toString();
    ui->label_songname->setText(musicName);
    qDebug()<<"当前播放:"<<musicName;

//    cellIndex = pModel->index(index.row(), index.column()+1);
//    data = pModel->data(cellIndex);
//    QString singerName = data.toString();
//    strcpy(Song.singerName, singerName.toStdString().c_str());

//    mainwindow->song_List.insert(playlist->currentIndex()+1, Song);//插入到播放列表信息

}

void UserWindow::on_pushButton_star_clicked() // 播放暂停按钮槽函数
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
        ui->pushButton_star->setStyleSheet(
                    "QPushButton {"
                    "    border: none;"
                    "    background-color: transparent;"
                    "    image: url(:/播放.png);"
                    "}"
                    );

    }
    else if(player->state() == QMediaPlayer::PausedState)
    {
        player->play();
        ui->pushButton_star->setStyleSheet(
                    "QPushButton {"
                    "    border: none;"
                    "    background-color: transparent;"
                    "    image: url(:/暂停.png);"
                    "}"
                    );
    }
}


void UserWindow::on_pushButton_priv_clicked() // 播放上一首歌按钮
{

    int index = playlist->currentIndex();
    player->setPlaylist(playlist);
    playlist->setCurrentIndex(index);
    if(playlist->currentIndex()>0)
    {
        playlist->previous();
        qDebug()<<"当前歌曲下标:"<<playlist->currentIndex()<<endl;
        player->play();

    }
    else
    {
        qDebug()<<"已是最后一首"<<endl;
        player->play();
    }
}

void UserWindow::on_pushButton_next_clicked() // 下一首歌按钮槽函数
{
    int index = playlist->currentIndex();
    player->setPlaylist(playlist);
    playlist->setCurrentIndex(index);
    playlist->next();
    qDebug()<<"当前歌曲下标:"<<playlist->currentIndex()<<endl;
    player->play();
}

void UserWindow::on_horizontalSlider_actionTriggered(int action) // 进度条滑动
{
    ui->label_start_time->setText(millisecondsToTimeString(ui->horizontalSlider->value()));
}

void UserWindow::on_horizontalSlider_sliderReleased() // 滑块拖动
{
    player->setPosition(ui->horizontalSlider->value());
}

void UserWindow::on_pushButton_2_clicked() // 设置静音
{
    if(player->volume() != 0)
    {
        player->setVolume(0);
        ui->pushButton_2->setStyleSheet(
                    "QPushButton#pushButton_2 {"
                    "    border: none;"
                    "    background-color: transparent;"
                    "    image: url(:/静音.png);"
                    "}"
                    );
    }
    else
    {
        player->setVolume(ui->horizontalSlider_volume->value());
        ui->pushButton_2->setStyleSheet(
                    "QPushButton#pushButton_2 {"
                    "    border: none;"
                    "    background-color: transparent;"
                    "    image: url(:/音量.png);"
                    "}"
                    );
    }
}

void UserWindow::on_horizontalSlider_volume_actionTriggered(int action) //滑动调节音量
{
    player->setVolume(ui->horizontalSlider_volume->value());
}

void UserWindow::on_horizontalSlider_volume_sliderReleased() // 点击调节音量
{
    ui->horizontalSlider_volume->setValue(ui->horizontalSlider_volume->value());
    player->setVolume(ui->horizontalSlider_volume->value());
}

void UserWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
//需添加头文件 <QPainter>和 <QStyleOption>




void UserWindow::mousePressEvent(QMouseEvent *event)
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

void UserWindow::mouseMoveEvent(QMouseEvent *event)
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

void UserWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_leftMousePressed = false;//释放鼠标，标志位置为假
}

void UserWindow::showUserWindow() // show函数
{
    QMainWindow::show();
}

void UserWindow::playMyFavorite(int musicId, QString musicName) // 播放我喜欢的音乐
{
    player->setPlaylist(playlist);
    int targetIndex = -1;
    for (int i = 0; i < playlist->mediaCount(); ++i)
    {
        QMediaContent media = playlist->media(i);
        QString urlString = media.canonicalUrl().toString();
        QString musicIdStr = QString::number(musicId);
            if (urlString.contains(musicIdStr))
            {
                qDebug()<<"找到播放列表这首歌";
                targetIndex = i;
                break;
            }
        }


    // 如果找到了对应的歌曲索引
    if(targetIndex != -1)
    {
        player->setPlaylist(playlist);
        playlist->setCurrentIndex(targetIndex);
        player->play();
        ui->label_songname->setText(musicName);
    }
    else
    {
        qDebug()<<"播放列表没有这首歌";
    }

}

void UserWindow::on_pushButton_minisize_clicked() // 最小化窗口
{
    this->showMinimized();
}

void UserWindow::on_pushButton_closewin_clicked() // 关闭窗口
{
    this->close();
}

void UserWindow::on_listWidget_2_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    QString text = current->text();
        if(text == "我喜欢的音乐")
        {
            pLocal->hide();
            pMyfavor->showFavoriteWindow();
        }
        else if(text == "本地音乐")
        {
            pMyfavor->hide();
            pLocal->showLocalMusic();
        }
}

