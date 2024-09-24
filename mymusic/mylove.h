#ifndef MYLOVE_H
#define MYLOVE_H

#include <QWidget>
#include<QFile>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QModelIndex>
#include"customdelegete.h"
#include"globalplayer.h"
class MainWindow;
namespace Ui {
class Mylove;
}

class Mylove : public QWidget
{
    Q_OBJECT

public:
    explicit Mylove(MainWindow *mainWidow,QWidget *parent = nullptr);
    ~Mylove();
    void showMy_love();//显示我的喜欢列表
    void send_singal();//发送信号
private slots:
    void on_pushButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::Mylove *ui;
    MainWindow *mainWidow;
    QStandardItemModel * pModel;
    QStringList titles;
    QMediaPlaylist * playlist;
    QMediaPlayer * player;
};

#endif // MYLOVE_H
