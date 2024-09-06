#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include "message.h"
#include "sqliteoperator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void onConnect();
    void onAccept();
    void onDisConnect();
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *client;
    sqliteoperator *sqliteOP;
    QStringList userName;
};
#endif // MAINWINDOW_H
