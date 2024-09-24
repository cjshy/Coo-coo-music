#ifndef SQLTEXT_H
#define SQLTEXT_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class sqltext
{
public:
    sqltext();
    bool openDb(void);//打开数据库
    void closeDb(void);//关闭数据库
    void runsql(QString sql);//执行函数
    void searchsql(QSqlQuery &sqlQuery,QString sql);//查询函数
    bool isTableExists(const QString &tablename);//判断表是否存在
    char *getUsernameById(const char *userId);//得到用户名
private:
    QSqlDatabase database;
};

#endif // SQLTEXT_H
