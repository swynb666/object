#include "regwindow.h"
#include "ui_regwindow.h"

RegWindow::RegWindow(MainWindow *mainwindow, QWidget *parent) :
    QMainWindow(parent),mainwindow(mainwindow),
    ui(new Ui::RegWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);//隐藏最大最小化等按键
    setAttribute(Qt::WA_TranslucentBackground);
    //主窗口样式
    this->setStyleSheet(
                "QMainWindow {"
                "background-color: rgb(209, 209, 209);" /* 背景颜色（灰色） */
                "border-radius: 10px;"                  /* 四角圆角的直径 */
                "border: none;"
                "background-image: url(:/登录页背景.png);" /* 背景图片 */
                "background-repeat: no-repeat;"          /* 不重复 */
                "background-position: center;"           /* 居中 */
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
    //缩小窗口按钮样式
    ui->pushButton_miniSizeWin->setStyleSheet(
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
    ui->pushButton_closeWin->setStyleSheet(
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

    //设置头像label为圆形且不覆盖边框
    ui->label_picture->setStyleSheet("border-image: url(:/音乐.png);"
                             "border:3px solid black;"
                             "border-radius: 64px;"
                             );

    //注册账号LineEdit样式
    ui->lineEdit->setStyleSheet(
                "QLineEdit {"
                "color: rgba(255, 255, 255);"
                "border-radius: 10px;"          // 边框圆角
                "padding: 5px;"                 // 内边距
                "background-color: rgba(255, 255, 255, 128);" // 背景颜色（白色，半透明）
                "}"
                );
    //注册密码LineEdit_2样式
    ui->lineEdit_2->setStyleSheet(
                "QLineEdit {"
                "color: rgba(255, 255, 255);"
                "border-radius: 10px;"          // 边框圆角
                "padding: 5px;"                 // 内边距
                "background-color: rgba(255, 255, 255, 128);" // 背景颜色（白色，半透明）
                "}"
                );
    //注册确认按钮样式
    ui->pushButton_Ensure->setStyleSheet(
                "QPushButton {"
                "background-color: #130454;"
                "border-radius:10px;"
                "color: rgb(255, 255, 255);"
                "}"
                );
    client = mainwindow->client;
}

RegWindow::~RegWindow()
{
    delete ui;
}

void RegWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void RegWindow::mousePressEvent(QMouseEvent *event)
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

void RegWindow::mouseMoveEvent(QMouseEvent *event)
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

void RegWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_leftMousePressed = false;//释放鼠标，标志位置为假
}

void RegWindow::on_pushButton_miniSizeWin_clicked()
{
    this->showMinimized();
}

void RegWindow::on_pushButton_closeWin_clicked()
{
    this->close();
}

void RegWindow::on_pushButton_Ensure_clicked()
{
    QMSG msg;
    REG reg;
    strcpy(reg.name, ui->lineEdit->text().toStdString().c_str());
    strcpy(reg.passwd, ui->lineEdit_2->text().toStdString().c_str());
    if(ui->radioButton->isChecked())
    {
        reg.sex = true; //男
    }
    else
    {
        reg.sex = false; //女
    }
    memcpy(msg.buf, &reg, sizeof(reg));
    msg.type = 1; // 设置类型1表示注册请求
    client->write((char *)&msg, sizeof(msg));
}
