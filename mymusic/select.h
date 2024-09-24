#ifndef SELECT_H
#define SELECT_H

#include <QWidget>
#include<QDebug>
#include<QMouseEvent>
#include<QPixmap>
#include<QPalette>
#include<QPainter>
#include<QFile>
#include<QTextStream>
#include<QListView>
#include<QStringListModel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include<QListWidget>
#include"search.h"
#include"mylove.h"
#include"recent_play.h"
#include"rotatingpushbutton.h"
class MainWindow;
namespace Ui {
class Select;
}

class Select : public QWidget
{
    Q_OBJECT

public:
    explicit Select(MainWindow *mainWidow,QWidget *parent = nullptr);
    ~Select();
    void resizeEvent(QResizeEvent *event);
    void updataBackground();
    //边框拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void revolve();//唱片旋转
    void show_SongName();//显示歌名
    void showPlaylistWithAnimation();
    void loadPlaylistFromSongs();
    void hidePlaylistWithAnimation();
    void highlightCurrentPlayingSong();
private slots:
    void onPlayerStateChanged(QMediaPlayer::State);//播放图标设置

    void my_durationChanged(qint64 duration);

    void my_positionChanged(qint64 position);

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_listView_clicked(const QModelIndex &index);

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_pifu_clicked();

    void on_horizontalSlider_3_sliderMoved(int position);

    void on_pushButton_13_clicked();

    void on_pushButton_2_clicked();

    void onPlaylistItemClicked(QListWidgetItem *item);

    void on_click_Myclear();
private:
    Ui::Select *ui;
    Search *search;
    Mylove *mylove;
    QStringList list;
    QPoint diff_pos;  // 鼠标和窗口的相对位移
    QPoint window_pos;
    QPoint mouse_pos;
    QStringListModel *listmodel;
    QMediaPlaylist * playlist;
    QMediaPlayer * player;
    MainWindow *mainWidow;
    Recent_play *recent;
    QMediaPlaylist::PlaybackMode currentPlaybackMode = QMediaPlaylist::Sequential; // 修正后的类型声明
    QListWidget *m_liebiao;
    QWidget *widget;
};

#endif // SELECT_H
