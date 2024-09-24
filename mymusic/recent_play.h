#ifndef RECENT_PLAY_H
#define RECENT_PLAY_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QModelIndex>
#include<QDebug>
#include"customdelegete.h"
class MainWindow;
namespace Ui {
class Recent_play;
}

class Recent_play : public QWidget
{
    Q_OBJECT

public:
    explicit Recent_play(MainWindow *mainwindow,QWidget *parent = nullptr);
    ~Recent_play();
    void showTable();
    bool checkFavorit(const char *user_id,const char *song_id);//判断该用户是否收藏该歌曲
private slots:
    void on_pushButton_2_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

private:
    Ui::Recent_play *ui;
    MainWindow *mainWidow;
    QStandardItemModel * pModel;
    QStringList titles;
    QMediaPlaylist * playlist;
    QMediaPlayer * player;
};

#endif // RECENT_PLAY_H
