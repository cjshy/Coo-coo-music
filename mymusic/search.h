#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QModelIndex>
#include<QAction>
class MainWindow;
namespace Ui {
class Search;
}

class Search : public QWidget
{
    Q_OBJECT

public:
    struct song
    {
        QString songname;   //歌曲名
        QString singer;     //歌手
        QString album_name; //专辑
        qint64 Time;      //时间
        int music_id;       //音乐id
    };
    explicit Search(MainWindow *mainWidow,QWidget *parent = nullptr);
    ~Search();
    void parseJson(QByteArray jsonByte);//json字符串转换
    void getCurrentsong();//获得正在播放的歌名
    QString gettotaltime(qint64 Time);//时间转换
    bool checkFavorit(const char *user_id,const char *song_id);//判断该用户是否收藏该歌曲
    void insertToUser();//插入数据库
    void deleteNonExistentSongs(); // 删除
    void insertRecentPlay(const song &currentSong);//最近播放
private slots:
    void replyFinished(QNetworkReply *reply);

    void on_pushButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::Search *ui;
    QNetworkAccessManager *manager;
    QStandardItemModel * pModel;
    QStringList titles;
    QMediaPlaylist * playlist;
    QMediaPlayer * player;
     MainWindow *mainWidow;
};

#endif // SEARCH_H
