#include "sqliteoperator.h"

//构造函数中初始化数据库对象，并且建立数据库
sqliteoperator::sqliteoperator()
{
    //数据库是否已经连上了
    if(QSqlDatabase::contains("qt_sql_default_connection") == true)
    {
        //qt_sql_default_connection 代表当前数据库连接
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        //建立和sqlite数据库连接
        database = QSqlDatabase::addDatabase("QSQLITE");
        //设置数据库文件的名字
        database.setDatabaseName("MyDataBase.db");
    }

}

//打开数据库
bool sqliteoperator::openDb()
{
    if(!database.open())
    {
        qDebug()<<"Error: failed to connect database"<<endl;
    }
    else
    {}
}

//关闭数据库
void sqliteoperator::closeDb()
{
    database.close();
}

//通用执行函数
void sqliteoperator::normalExec(QString sql)
{
    QSqlQuery sqlQuery;
    if(!sqlQuery.exec(sql))
    {
        qDebug()<<sqlQuery.lastError();
        qDebug()<<"执行失败";
    }
    else
    {
        qDebug()<<"ok"<<endl;
    }
}

//通用查询函数
void sqliteoperator::normalQuery(QSqlQuery &sqlQuery, QString sql)
{
    if(!sqlQuery.exec(sql))
    {
        qDebug()<<sqlQuery.lastError();

    }
    else
    {
        qDebug()<<"ok"<<endl;
    }
}
