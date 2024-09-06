#ifndef SQLITEOPERATOR_H
#define SQLITEOPERATOR_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

class sqliteoperator
{
public:
    sqliteoperator();

    //打开数据库
    bool openDb();

    //关闭数据库
    void closeDb();

    //通用执行函数
    void normalExec(QString sql);

    //通用查询函数
    void normalQuery(QSqlQuery & sqlQuery, QString sql);
private:
    QSqlDatabase database; // 用于建立和数据库的连接
};

#endif // SQLITEOPERATOR_H
