#include "myfavorite.h"
#include "ui_myfavorite.h"
#include "mainwindow.h"


MyFavorite::MyFavorite(MainWindow *mainWindow, QWidget *parent) :
    QWidget(parent),mainWindow(mainWindow),
    ui(new Ui::MyFavorite)
{
    ui->setupUi(this);
    client = mainWindow->client;
    qDebug()<<"MyFavoriteWin当前："<<mainWindow->reg.name<<endl;
    pModel = new QStandardItemModel(ui->tableView); // 实例化模型，并指定模型控件
    ui->tableView->setModel(pModel);
    ui->tableView->setItemDelegateForColumn(4,new customDelegete);
    titles << "歌名" << "歌手" << "专辑" << "时长" << "收藏" << "Id";

    player = mainWindow->player;
    myFaovorlist = mainWindow->myFaovorlist;

    //中间tableview样式设置
    QHeaderView * pHeaderView = ui->tableView->horizontalHeader();
    pHeaderView->setStyleSheet(
                "QHeaderView::section{"
                "background-color: #2B2B2B;"
                "    color: #B5B5B5;" // 设置字体颜色
                "}"
                );
    QHeaderView * pHeaderVerticalView = ui->tableView->verticalHeader();
    pHeaderVerticalView->setStyleSheet(
                "QHeaderView::section{"
                "background-color: #2B2B2B;"
                "    color: #B5B5B5;" // 设置字体颜色
                "}"
                );

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
    // 隐藏最后一列
    ui->tableView->hideColumn(ui->tableView->model()->columnCount() - 1);
    // 关闭表头的鼠标点击功能
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
}


MyFavorite::~MyFavorite()
{
    delete ui;
}

void MyFavorite::showFavoriteWindow()
{
    QWidget::show();
    // 隐藏最后一列
    ui->tableView->hideColumn(ui->tableView->model()->columnCount() - 1);
    QMSG msg;
    memcpy(msg.buf,  &mainWindow->reg, sizeof(REG));
    msg.type = 3;
    client->write((char *)&msg, sizeof(msg));
    client->waitForBytesWritten();
}


void MyFavorite::getMyfavorite()
{
    pModel->removeRows(0, pModel->rowCount());
    for(int i=0; i<mainWindow->songList.size(); i++)
    {
        pModel->setHorizontalHeaderLabels(titles);
        QList<QStandardItem *> sti;
        sti<< new QStandardItem(mainWindow->songList[i].songName)<< new QStandardItem(mainWindow->songList[i].singerName)<< new QStandardItem(mainWindow->songList[i].albumName)<<new QStandardItem(QString().number(mainWindow->songList[i].duration2))<<new QStandardItem("已收藏")<<new QStandardItem(QString().number(mainWindow->songList[i].musicId)) ;
        // 设置每个 item 的对齐方式为居中
        for (QStandardItem *item : sti)
        {
            item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        }
        // 隐藏最后一列
        ui->tableView->hideColumn(ui->tableView->model()->columnCount() - 1);
        pModel->appendRow(sti);


    }
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


void MyFavorite::on_tableView_clicked(const QModelIndex &index)
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
    strcpy(fSong.userName, mainWindow->reg.name);

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
        mainWindow->checkList.removeOne(fSong.Song.musicId);
        pModel->removeRow(row);
    }
}

void MyFavorite::on_tableView_doubleClicked(const QModelIndex &index)
{
    //构建歌曲播放链接
    QModelIndex cellIndex = pModel->index(index.row(),index.column());
    QVariant data = pModel->data(cellIndex);
    cellIndex = pModel->index(index.row(), index.column()+5);
    data = pModel->data(cellIndex);
    int musicId = data.toInt();

    data = pModel->data(cellIndex);
    cellIndex = pModel->index(index.row(), index.column()+0);
    data = pModel->data(cellIndex);
    QString musicName = data.toString();
    mainWindow->userWindow->playMyFavorite(musicId, musicName);
}
