#include "localmusic.h"
#include "ui_localmusic.h"


LocalMusic::LocalMusic(MainWindow *mainWindow, QWidget *parent) :
    QWidget(parent),mainwindow(mainWindow),
    ui(new Ui::LocalMusic)
{
    ui->setupUi(this);

    player = mainWindow->player;
    plocalList = new QMediaPlaylist;
    plocalList->setPlaybackMode(QMediaPlaylist::Loop);
    pModel = new QStandardItemModel;
    ui->tableView->setModel(pModel);
    ui->tableView->setItemDelegateForColumn(4,new customDelegete);
    titles << "歌名";

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

}

LocalMusic::~LocalMusic()
{
    delete ui;
}

void LocalMusic::showLocalMusic()
{
    QWidget::show();
    pModel->removeRows(0, pModel->rowCount());
    QString directoryPath = "C:/Users/mili1/Documents/project_music/localMusic";
    QDir dir(directoryPath);

    if (!dir.exists())
    {
        qWarning() << "Directory does not exist:" << directoryPath;
        return;
    }

    // 获取目录下的所有文件和子目录
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    foreach (const QFileInfo &fileInfo, fileInfoList)
    {
        qDebug() << "File:" << fileInfo.fileName();
        pModel->setHorizontalHeaderLabels(titles);
        QList<QStandardItem *> sti;
        sti<< new QStandardItem(fileInfo.fileName());
        // 设置每个 item 的对齐方式为居中
        for (QStandardItem *item : sti)
        {
            item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        }
        pModel->appendRow(sti);

        // 将文件路径添加到播放列表
        plocalList->addMedia(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
    ui->tableView->setColumnWidth(0, 1024);
    // 设置每一行的高度
    for (int row = 0; row < pModel->rowCount(); ++row) {
        ui->tableView->setRowHeight(row, 50); // 设置每一行的高度为30像素
    }
}


void LocalMusic::on_tableView_doubleClicked(const QModelIndex &index)
{
    QString fileName = index.data().toString();
        QString filePath = "C:/Users/mili1/Documents/project_music/localMusic/" + fileName;
        int mediaIndex = plocalList->mediaCount();

        for (int i = 0; i < plocalList->mediaCount(); ++i)
        {
            if (plocalList->media(i).canonicalUrl().toLocalFile() == filePath)
            {
                mediaIndex = i;
                break;
            }
        }

        player->setPlaylist(plocalList);
        plocalList->setCurrentIndex(mediaIndex);
        player->play();

}
