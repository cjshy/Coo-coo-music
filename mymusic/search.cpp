#include "search.h"
#include "ui_search.h"
#include"mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include"customdelegete.h"
#include"globalplayer.h"
Search::Search(MainWindow *mainWidow,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Search),mainWidow(mainWidow)
{
    ui->setupUi(this);
    //通过Action绑定快捷键
    QAction *enter_action = new QAction(this);
    enter_action->setShortcut(QKeySequence(Qt::Key_Return));
    connect(enter_action,&QAction::triggered,ui->pushButton,&QPushButton::click);
    addAction(enter_action);
    //设置歌单
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
    pModel = new QStandardItemModel(ui->tableView);//实例化模型，并且指定模型依赖的控件
    ui->tableView->setModel(pModel);
    ui->tableView->setItemDelegateForColumn(4,new customdelegete);
    titles << "歌名" << "歌手" << "专辑" << "时长" <<"收藏"<<"ID";
    //设置play
    player = GlobalPlayer::instance()->player();
    playlist = GlobalPlayer::instance()->playlist();
    player->setPlaylist(playlist);
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section { background: transparent; };");
    //


}

Search::~Search()
{
    delete ui;
}

void Search::replyFinished(QNetworkReply *reply)
{
    qDebug()<<"网页响应已完成"<<endl;
    QVariant ret = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug()<<"网页返回状态为"<<ret<<endl;
    if(ret == 200)//200说明页面访问成功
    {
        QByteArray data = reply->readAll();//读取所有的网络数据
        parseJson(data);
    }
}


void Search::parseJson(QByteArray jsonByte)
{
    QString songname_original2; //歌曲名
    QString singername2;        //歌手
    QString album_name2;        //专辑
    int duration2;          	//时间
    int duration3;
    int music_id;               //音乐id

    QJsonParseError json_error;//JSON解析错误对象
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonByte, &json_error);

    if (json_error.error == QJsonParseError::NoError)//判断解析过程是否有错误
    {
        if (parse_doucment.isObject())//判断文档是否是一个json对象
        {
            QJsonObject rootObj = parse_doucment.object();//获取文档JSON对象
            if (rootObj.contains("result"))//对象中是否包含键result
            {
                QJsonValue valuedata = rootObj.value("result");//获得result对应的值
                if (valuedata.isObject())//result对应的值是否是一个对象
                {
                    QJsonObject valuedataObject = valuedata.toObject();//把QJsonValue转为JsonObject对象
                    if (valuedataObject.contains("songs"))
                    {
                        QJsonValue valueArray = valuedataObject.value("songs");
                        if (valueArray.isArray())
                        {
                            //把结果转为QJsonArray数组才能遍历
                            QJsonArray array = valueArray.toArray();
                            int size = array.size();//(size>36?36:size)
                            for (int i = 0; i <=size; i++)
                            {
                                QJsonValue value = array.at(i);
                                if (value.isObject())
                                {
                                    //把数组的第i个元素变成一个JSON对象
                                    QJsonObject object = value.toObject();
                                    if (object.contains("album"))//专辑1
                                    {
                                        QJsonValue album_Object_1 = object.value("album");
                                        if (album_Object_1.isObject())
                                        {
                                            QJsonObject album_Object_1_tmp=album_Object_1.toObject();
                                            if(album_Object_1_tmp.contains("name"))
                                            {
                                                QJsonValue ID_value = album_Object_1_tmp.value("name");
                                                if (ID_value.isString())
                                                {
                                                    album_name2 = ID_value.toString();
                                                }
                                            }
                                        }
                                    }

                                    if (object.contains("artists"))//歌手
                                    {
                                        QJsonValue artists_value_tmp = object.value("artists");
                                        if(artists_value_tmp.isArray())
                                        {
                                            QJsonArray artists_array = artists_value_tmp.toArray();
                                            int artists_size=artists_array.size();
                                            for(int k=0;k<artists_size;k++)
                                            {
                                                QJsonValue artists_name=artists_array.at(k);
                                                if(artists_name.isObject())
                                                {
                                                    QJsonObject artists_name_object=artists_name.toObject();
                                                    if(artists_name_object.contains("name"))
                                                    {
                                                        QJsonValue artistsname = artists_name_object.take("name");
                                                        if (artistsname.isString())
                                                        {
                                                            if(k==0)
                                                            {
                                                                singername2 = artistsname.toString();
                                                            }
                                                            else
                                                            {
                                                                singername2 = singername2 + "/" +artistsname.toString();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if (object.contains("name"))//歌曲名
                                    {
                                        QJsonValue Album_ID_value = object.take("name");
                                        if (Album_ID_value.isString())
                                        {
                                            songname_original2 = Album_ID_value.toString();
                                        }
                                    }
                                    if (object.contains("duration"))//时长
                                    {
                                        QJsonValue AlbumID_value = object.take("duration").toInt();
                                        duration2 = AlbumID_value.toInt();
                                        duration3 = duration2;
                                        duration2=duration2/1000;
                                    }
                                    if (object.contains("id"))//歌曲id
                                    {
                                        QJsonValue FileHash_value = object.take("id");
                                        if (!FileHash_value.isNull())
                                        {
                                            //用Vector保存每首歌曲的album_id
                                            music_id=FileHash_value.toInt();
                                        }
                                    }
                                    pModel->setHorizontalHeaderLabels(titles);
                                    QList<QStandardItem *> sti;
                                    // 设置 "ID" 列不显示
                                    ui->tableView->setColumnHidden(5,true);
                                    //转换music_id
                                    QString music_id2 =QString::number(music_id);
                                    // 检查是否已收藏
                                    bool isFavorite = checkFavorit(mainWidow->curNameid, music_id2.toStdString().c_str());
                                    //设置pmodel中内容
                                    sti<< new QStandardItem(songname_original2)<< new QStandardItem(singername2)<< new QStandardItem(album_name2)<<new QStandardItem(gettotaltime(duration3))<<new QStandardItem(isFavorite ? "已收藏" : "未收藏" )<<new QStandardItem(QString::number(music_id));
                                    pModel->appendRow(sti);

                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << json_error.errorString();
    }
}

void Search::getCurrentsong()
{
    qDebug() << "---音乐---";
    int currentIndex = playlist->currentIndex(); // 获取当前播放的索引
    if (currentIndex >= 0 && currentIndex < mainWidow->song_List.size()) {
        MainWindow::song currentMainSong = mainWidow->song_List[currentIndex]; // 获取当前播放的歌曲信息
        qDebug() << "现在播放的歌名：" << currentMainSong.songname<<"时长为："<<currentMainSong.Time; // 输出歌曲名     
        // 转换为 Search::song 类型
        song currentSong;
        currentSong.songname = currentMainSong.songname;
        currentSong.singer = currentMainSong.singer;
        currentSong.album_name = currentMainSong.album_name;
        currentSong.Time = currentMainSong.Time;
        currentSong.music_id = currentMainSong.music_id;

        insertRecentPlay(currentSong); // 插入最近播放的歌曲
    } else {
        qDebug() << "没有这首歌";
    }
}

void Search::insertRecentPlay(const song &currentSong)
{
    // 检查当前歌曲是否已经存在于 recent_play 表中
    QString checkSql = QString("SELECT COUNT(*) FROM playback_%1 WHERE song_id='%2'")
            .arg(mainWidow->curNameid)
            .arg(currentSong.music_id);

    QSqlQuery query;
    mainWidow->sql->searchsql(query, checkSql);

    if (query.next() && query.value(0).toInt() > 0)
    {
        // 记录已经存在，无需插入
        qDebug() << "记录已存在：" << currentSong.songname;
        return;
    }

    // 构建插入记录的 SQL 语句
    QString insertSql = QString("INSERT INTO playback_%1 (song_name, album, singer, Time, song_id) "
                                "VALUES ('%2', '%3', '%4', '%5', '%6')")
            .arg(mainWidow->curNameid)
            .arg(currentSong.songname)
            .arg(currentSong.album_name)
            .arg(currentSong.singer)
            .arg(gettotaltime(currentSong.Time * 1000))
            .arg(currentSong.music_id);

    // 执行插入操作
    mainWidow->sql->runsql(insertSql); // 直接调用，不再检查返回值
    qDebug() << "尝试插入：" << currentSong.songname;
}

QString Search::gettotaltime(qint64 Time)
{
    QTime time(0,0);
    QTime totaltime = time.addSecs(Time / 1000);
    QString formation = totaltime.toString("mm:ss");
    return formation;
}

bool Search::checkFavorit(const char *user_id, const char *song_id)
{
    // 遍历 love_list，检查 song_id 是否存在
    for (const MainWindow::song_love &love :mainWidow->love_list) {
        // 检查 user_id 和 song_id
        if (strcmp(love.user_id, user_id) == 0 && strcmp(love.song_id, song_id) == 0) {
            return true; // 找到匹配的收藏记录，返回 true
        }
    }

    return false; // 未找到匹配的收藏记录，返回 false
}

void Search::on_pushButton_clicked()//搜索
{  
    pModel->clear();
    QString key = ui->lineEdit->text();
    QString url = QString("http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s=%1&type=1&offset=0&total=true&limit=10").arg(key);
    manager->get(QNetworkRequest(QUrl(url)));
}

void Search::on_tableView_clicked(const QModelIndex &index)//收藏
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
}


void Search::on_tableView_doubleClicked(const QModelIndex &index)
{
    // 获取点击行的所有列的值
    QString songname = pModel->data(pModel->index(index.row(), 0)).toString(); // 歌名
    QString singer = pModel->data(pModel->index(index.row(), 1)).toString(); // 歌手
    QString album_name = pModel->data(pModel->index(index.row(), 2)).toString(); // 专辑
    QString duration = pModel->data(pModel->index(index.row(), 3)).toString(); // 时长
    QString music_id = pModel->data(pModel->index(index.row(), 5)).toString(); // ID

    // 插入链表
    MainWindow::song _song;
    _song.album_name = album_name;
    _song.singer = singer;
    _song.songname = songname;
    _song.Time = duration.toInt(); // 假设 duration 是字符串形式的整数
    _song.music_id = music_id.toInt();
    mainWidow->song_List.push_back(_song);

    // 构建歌曲播放链接
    QString play_url = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(music_id);
    playlist->addMedia(QUrl(play_url)); // 把歌曲加入到播放列表

    //设置当前点击为播放的歌曲
    playlist->setCurrentIndex(playlist->mediaCount()-1);
    player->play();

}
