#include "mylove.h"
#include "ui_mylove.h"
#include"mainwindow.h"
#include <chrono>
#include <thread>
Mylove::Mylove(MainWindow *mainWidow,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Mylove),mainWidow(mainWidow)
{
    ui->setupUi(this);
    ui->pushButton->setShortcut(QKeySequence(Qt::Key_Return));//直接在按钮上绑定快捷键
    //设置歌单
    pModel = new QStandardItemModel(ui->tableView);//实例化模型，并且指定模型依赖的控件
    ui->tableView->setModel(pModel);
    ui->tableView->setItemDelegateForColumn(4,new customdelegete);
    titles << "歌名" << "歌手" << "专辑" << "时长" <<"收藏"<<"ID";
    //设置play
    player = GlobalPlayer::instance()->player();
    playlist = GlobalPlayer::instance()->playlist();
    player->setPlaylist(playlist);

}

Mylove::~Mylove()
{
    delete ui;
}

void Mylove::showMy_love()//显示我的喜欢列表
{

    qDebug()<<"----我的喜欢-----";
    qDebug()<<mainWidow->lastDisplayedCount<<"---------";
    pModel->setHorizontalHeaderLabels(titles);
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section { background: transparent; };");
    // 设置 "ID" 列不显示
    ui->tableView->setColumnHidden(5,true);
    // 获取 love_list 数据
    QList<MainWindow:: song_love> loveList = mainWidow->love_list;

    // 清空模型
    pModel->removeRows(0, pModel->rowCount());

    // 填充模型
    for (const MainWindow:: song_love &love : loveList) {
        QList<QStandardItem*> row;
        row << new QStandardItem(love.song_name)
            << new QStandardItem(love.singer)
            << new QStandardItem(love.album)
            << new QStandardItem(love.time)
            << new QStandardItem("已收藏") // 假设 "收藏" 列的值为 "收藏"
            << new QStandardItem(love.song_id);
        pModel->appendRow(row);
    }


}

void Mylove::send_singal()//发送信号
{
    Qmesg msg;
    msg.flag = 5; // 我的喜欢列表
    song_text Song(mainWidow->curNameid, "NULL");
    memcpy(msg.data, &Song, sizeof(song_text));
    mainWidow->clientSocket->write((char *)&msg, sizeof(Qmesg));
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Mylove::on_pushButton_clicked()//搜索
{
    QString key = ui->lineEdit->text();
    if (key.isEmpty()) {
        showMy_love();
        return;
    }

    // 遍历模型中的每一行
    for (int row = 0; row < pModel->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < pModel->columnCount(); ++col) {
            QStandardItem *item = pModel->item(row, col);
            if (item && item->text().contains(key, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        // 设置行可见性
        ui->tableView->setRowHidden(row, !match);
    }
}

void Mylove::on_tableView_clicked(const QModelIndex &index)//收藏
{
    // 获取点击行的所有列的值
    std::string songname =  pModel->data(pModel->index(index.row(), 0)).toString().toStdString(); // 歌名
    std::string singer = pModel->data(pModel->index(index.row(), 1)).toString().toStdString(); // 歌手
    std::string album_name = pModel->data(pModel->index(index.row(), 2)).toString().toStdString(); // 专辑
    std::string duration = pModel->data(pModel->index(index.row(), 3)).toString().toStdString(); // 时长
    std::string music_id = pModel->data(pModel->index(index.row(), 5)).toString().toStdString(); // ID

    qDebug()<<"歌名："<<songname.c_str()<<"歌曲id："<<music_id.c_str()<<"用户名："<<mainWidow->curNameid;
    Qmesg msg;
    song_text Song(mainWidow->curNameid,music_id.c_str());
    strcpy(Song.music_name,songname.c_str());
    strcpy(Song.singer,singer.c_str());
    strcpy(Song.time,duration.c_str());
    strcpy(Song.zhuanji,album_name.c_str());
    memcpy(msg.data,&Song,sizeof(song_text));

    if(index.column() != 4)
    {
        return ;
    }

    QVariant ret = pModel->data(index);//获得索引处的模型数据
    if(ret == "未收藏")
    {
        pModel->setItem(index.row(),index.column(),new QStandardItem("已收藏"));
        msg.flag = 3;//我的喜欢
        mainWidow->clientSocket->write((char *)&msg,sizeof(Qmesg));

    }
    else if(ret == "已收藏")
    {
        pModel->setItem(index.row(),index.column(),new QStandardItem("未收藏"));
        msg.flag = 4;//取消收藏
        mainWidow->clientSocket->write((char *)&msg,sizeof(Qmesg));
    }

    mainWidow->clientSocket->waitForBytesWritten(1000);
    send_singal();//发送信号

}

void Mylove::on_tableView_doubleClicked(const QModelIndex &index) // 双击播放
{
    // 清空播放链表
    playlist->clear();

    // 清空歌曲链表
    mainWidow->song_List.clear();

    // 获取 QTableView 中的所有行
    int rowCount = pModel->rowCount();

    for (int row = 0; row < rowCount; ++row) {
        // 获取每行的数据
        QString songname = pModel->data(pModel->index(row, 0)).toString(); // 歌名
        QString singer = pModel->data(pModel->index(row, 1)).toString(); // 歌手
        QString album_name = pModel->data(pModel->index(row, 2)).toString(); // 专辑
        QString duration = pModel->data(pModel->index(row, 3)).toString(); // 时长
        QString music_id = pModel->data(pModel->index(row, 5)).toString(); // ID
        // 构建歌曲播放链接
        QString play_url = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(music_id);
        playlist->addMedia(QUrl(play_url)); // 把歌曲加入到播放列表

        // 插入链表
        int totalSeconds =mainWidow->convertDurationToSeconds(duration); // 获取总时长（秒）
        MainWindow::song _song;
        _song.album_name = album_name;
        _song.singer = singer;
        _song.songname = songname;
        _song.Time = totalSeconds;
        _song.music_id = music_id.toInt();
        mainWidow->song_List.push_back(_song);
    }

    // 设置当前点击的歌曲为播放歌曲
    playlist->setCurrentIndex(index.row());
    player->play();
}
