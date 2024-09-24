#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //服务器创建
    server = new QTcpServer(this);//创建服务器对象
    server->listen(QHostAddress::Any,9000);//设置端口号，若本机作为服务器QHostAddress可填any
    connect(server,SIGNAL(newConnection()),this,SLOT(acceptConnect()));
    //数据库
    sql = new sqltext;
    sql->openDb();
    if(!sql->isTableExists("user"))
    {
        sql->runsql("create table user(name varchar(128),passwd varchar(128),id varchar(128),phone varchar)");
    }
    if(!sql->isTableExists("user_song"))
    {
        sql->runsql("create table user_song(username varchar(128),user_id varchar(128),song_name varchar(128),album varchar(128),singer varchar(128),Time varchar(128),song_id varchar(128))");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getRandomId()//生成随机id
{
    const QString possibleChar("1234567890");
    QString randString;

    // 使用当前时间作为种子
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, possibleChar.length() - 1);

    for (int i = 0; i < 5; ++i) // 生成随机5位数id
    {
        int index = dis(gen);
        QChar nextchar = possibleChar.at(index);
        randString.append(nextchar);
    }

    return randString;
}

bool MainWindow::isIdExists(const QString &username, const QString &ID)//判断id是否存在
{
    QSqlQuery query;
    query.exec(QString("select * from user where name ='%1' and id ='%2'").arg(username).arg(ID));
    return query.next();
}

QString MainWindow::getCurretId(const QString &username)//得到最后生成的id
{
    int flage =0;
    QString str = getRandomId();
    qDebug()<<"生成的ID为："<<str;
    while(flage == 0)
    {
        if(isIdExists(username,str))
        {
            qDebug()<<"id已存在，准备生成新的id";
            flage = 0;
        }
        else
        {
            qDebug()<<"成功生成新id";
            flage =1;
        }
    }
    return str;
}

char *MainWindow::getUsernameById(const char *userId)
{
    QSqlQuery query;
    if (query.exec(QString("select name from user where id ='%1'").arg(userId)))
    {
        if (query.next())
        {
            QString username = query.value(0).toString();
            //            qDebug() << "找到的用户名: " << username;
            return strdup(username.toStdString().c_str()); // 使用 strdup 分配内存并返回 char*
        }
        else
        {
            qDebug() << "未找到匹配的用户ID: " << userId;
            return nullptr; // 如果未找到匹配的username，返回 nullptr
        }
    }
    else
    {
        qDebug() << "SQL 查询执行失败: " << query.lastError().text();
        return nullptr;
    }
}

void MainWindow::acceptConnect()
{
    //返回当前连接的客户端
    ClientSocket = server->nextPendingConnection();

    connect(ClientSocket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(ClientSocket,SIGNAL(disconnected()),this,SLOT(onDisconnect()));

}

void MainWindow::onReadyRead()//收取客户端信息
{
    if(ClientSocket)
    {
        Qmesg msg;
        memset(&msg,0,sizeof(msg));
        int len = ClientSocket->read((char *)&msg,sizeof(msg));
        qDebug()<<"收到"<<len<<"个字节的信息";

        if(msg.flag == 1)//注册
        {
            qDebug()<<"收到注册信息";
            user_text *user = (user_text *)msg.data;
            QString id = getCurretId(user->name);
            std::string id1 = id.toStdString();
            qDebug()<<user->name<<" "<<user->passwd<<" "<<id1.c_str()<<" "<<user->Phone;
            QSqlQuery sqlquery;
            sql->searchsql(sqlquery,QString("select * from  user where phone ='%1' ").arg(user->Phone));
            if(sqlquery.next())//手机号已存在
            {
                qDebug()<<"用户注册失败";
                ui->textEdit->append(QString("%1手机号已存在").arg(user->Phone));
                //回复客户端注册失败
                char buf[128] ="注册失败";
                ClientSocket->write(buf,sizeof(buf));
            }
            else
            {
                ui->textEdit->append(QString("收到用户名为%1的用户的注册信息，密码为%2,手机号码为%3,生成的账号为：%4").arg(user->name).arg(user->passwd).arg(user->Phone) .arg(id1.c_str()));
                QString sqls =QString("insert into user values('%1','%2','%3','%4')").arg(user->name).arg(user->passwd).arg(id1.c_str()).arg(user->Phone);
                sql->runsql(sqls);
                ui->textEdit->append(QString("%1用户注册成功").arg(user->name));
                //回复客户端注册成功
                char buf[128] ="注册成功";
                ClientSocket->write(buf,sizeof(buf));
                ClientSocket->waitForBytesWritten(5000);

                ClientSocket->write(id1.c_str(),128);
            }

        }
        else if(msg.flag == 2)//登录
        {
            user_text *user = (user_text *)msg.data;
            qDebug()<<"用户账号："<<user->name<<"用户密码： "<<user->passwd;
            ui->textEdit->append(QString("收到账户为%1的用户的登录信息").arg(user->name));
            QSqlQuery sqlQuery;//用于存放查询的结果
            sql->searchsql(sqlQuery,QString("select * from user where id ='%1' and passwd ='%2'").arg(user->name).arg(user->passwd));
            if(sqlQuery.next())
            {
                qDebug()<<"用户登录成功";
                ui->textEdit->append(QString("%1账户登陆成功").arg(user->name));
                //回复客户端登录成功
                char buf[128] ="登录成功";
                ClientSocket->write(buf,sizeof(buf));
                ClientSocket->waitForBytesWritten(1000);
                strcpy(user->Account,user->name);
                strcpy(user->name,getUsernameById(user->Account));
                qDebug()<<user->name;
                //                ClientSocket->write(user->Account,128);
                memcpy(msg.data,user,sizeof(user_text));
                user_text *user_2 = (user_text *)msg.data;
                qDebug()<<"user_2"<<user_2->name;
                ClientSocket->write((char *)&msg, sizeof(msg));

            }
            else
            {
                qDebug()<<"用户或密码错误";
                ui->textEdit->append(QString("%1用户登陆失败").arg(user->name));
                //回复客户端登录失败
                char buf[128] ="登录失败";
                ClientSocket->write(buf,sizeof(buf));
                ClientSocket->waitForBytesWritten(1000);
            }

        }
        else if(msg.flag == 3)//我的收藏
        {
            song_text *Song = (song_text *)msg.data;
            qDebug()<<"id为："<<Song->user_id<<"收藏了："<<Song->music_name<<"id 为："<<Song->music_id<<"的歌";
            ui->textEdit->append(QString("账号为%1的用户收藏了歌名为%2id为%3的歌").arg(Song->user_id).arg(Song->music_name).arg(Song->music_id));
            char temp_name[128] ;
            strcpy(temp_name,getUsernameById(Song->user_id));
            QString sqls =QString("insert into user_song values('%1','%2','%3','%4','%5','%6','%7')").arg(temp_name).arg(Song->user_id).arg(Song->music_name).arg(Song->zhuanji).arg(Song->singer).arg(Song->time) .arg(Song->music_id);
            sql->runsql(sqls);
        }
        else if(msg.flag == 4)//取消收藏
        {
            song_text *Song = (song_text *)msg.data;
            qDebug()<<"id为："<<Song->user_id<<"取消收藏了id为："<<Song->music_id<<"的歌";
            ui->textEdit->append(QString("账号为%1的用户取消收藏了id为%2的歌").arg(Song->user_id).arg(Song->music_id));
            qDebug()<<"歌名为："<<getUsernameById(Song->music_name);
            QString  sqls = QString("delete from user_song where user_id='%1' and song_id ='%2'").arg(Song->user_id).arg(Song->music_id);
            sql->runsql(sqls);
        }
        else if(msg.flag == 5)//我的喜欢
        {
            // 查询用户收藏的歌曲
            song_text *Song = (song_text *)msg.data;
            QSqlQuery sqlQuery;
            sql->searchsql(sqlQuery, QString("select * from user_song where user_id ='%1'").arg(Song->user_id));

            while(sqlQuery.next())
            {
                song_text song;
                strncpy(song.user_id, sqlQuery.value("user_id").toString().toStdString().c_str(), sizeof(song.user_id) - 1);
                strncpy(song.music_name, sqlQuery.value("song_name").toString().toStdString().c_str(), sizeof(song.music_name) - 1);
                strncpy(song.singer, sqlQuery.value("singer").toString().toStdString().c_str(), sizeof(song.singer) - 1);
                strncpy(song.time, sqlQuery.value("Time").toString().toStdString().c_str(), sizeof(song.time) - 1);
                strncpy(song.music_id, sqlQuery.value("song_id").toString().toStdString().c_str(), sizeof(song.music_id) - 1);
                strncpy(song.zhuanji, sqlQuery.value("album").toString().toStdString().c_str(), sizeof(song.zhuanji) - 1);
                // 获取用户名
                QString userName = getUsernameById(Song->user_id);
                //                qDebug() << "用户名: " << userName;
                strncpy(song.user_name, userName.toStdString().c_str(), sizeof(song.user_name) - 1);

                msg.flag = 51; // 标记为我的喜欢
                memcpy(msg.data, &song, sizeof(song));

                ClientSocket->write((char *)&msg, sizeof(msg));
            }
            qDebug()<<"-----------------";
        }
    }
}

void MainWindow::onDisconnect()//客户端断开连接
{
    if(ClientSocket)
    {
        qDebug()<<"客户端退出了";
    }
}

