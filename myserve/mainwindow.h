#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QDebug>
#include<QHostAddress>
#include<random>
#include<QString>
#include"comment.h"
#include"sqltext.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString getRandomId(void);//生成随机ID
    bool isIdExists(const QString &username,const QString &ID);//判断生成的ID是否已存在
    QString getCurretId(const QString &username);//得到最后的id
    char *getUsernameById(const char *userId);
private slots:
    void acceptConnect();
    void onReadyRead();
    void onDisconnect();
private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *ClientSocket;
    sqltext *sql;
};
#endif // MAINWINDOW_H
