#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpSocket>
#include<QMessageBox>
#include<QMouseEvent>
#include<QSystemTrayIcon>
#include<QMenu>
#include<QMovie>
#include<qdrawutil.h>
#include<QDir>
#include<QSignalMapper>
#include<QListWidget>
#include"select.h"
#include"sqltext.h"
#include"comment.h"
#include"zhuche.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class Select;
    friend class Zhuche;
    friend class Search;
    friend class Mylove;
    friend class Recent_play;
public:
    struct song
    {
        QString songname;   //歌曲名
        QString singer;     //歌手
        QString album_name; //专辑
        qint64 Time;      //时间
        int music_id;       //音乐id
    };
    struct song_love
    {
        char user_id[128];
        char user_name[128];
        char song_name[128];
        char song_id[128];
        char singer[128];
        char time[128];
        char album[128];
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //边框拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void processReceivedData(const char* data, qint64 size);
    void createUserPlaybackTable(const char *userId);
    int  convertDurationToSeconds(const QString &duration);//时间转换
    void paintEvent(QPaintEvent *e);//绘制背景阴影
    void User_file(QString account,QString name);//创建本地登录文件
    QStringList GetDirNameList(const QString &strDirpath);//用于获取用户文件下所有账号
private slots:
    void clientConnect();

    void Readyread();

    void on_pushButton_login_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void showWidget();

    void deleteaccount(int i);//删除下拉列表记录
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    Select *select;
    sqltext *sql;
    Zhuche *zhuche;
    QTcpSocket *clientSocket;
    QPoint diff_pos;  // 鼠标和窗口的相对位移
    QPoint window_pos;
    QPoint mouse_pos;
    char BUF[2048];
    QList<song> song_List;
    QList<song_love> love_list;
    char curNameid[128];//登录用户名
    int lastDisplayedCount; // 记录上次显示的数据量
    QSystemTrayIcon *systemtrayicon;//系统托盘
    QMenu *menu;
    QAction *m_pShowAction;
    QAction *m_pCloseAction;
    QMovie *m_movie;

    QVector<int> infoListsign;
    QSignalMapper * myMapper;
    QStringList infoList;               //用户账号
    QStringList infopasswd;             //用户密码
    QStringList icon;                   //头像保存地址
    QListWidget * m_AccountList;        //自定义下拉列表框
    bool ispasswd=false;                //判断是否是密码
};
#endif // MAINWINDOW_H
