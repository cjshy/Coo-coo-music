#include "select.h"
#include "ui_select.h"
#include"globalplayer.h"
#include"mainwindow.h"
#include"rotatingpushbutton.h"
#include <QPixmap>
#include <QBitmap>
#include <QMatrix>
#include <QIcon>
#include <QPushButton>

Select::Select(MainWindow *mainWidow,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Select),mainWidow(mainWidow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);//主窗口边框去掉
    setAttribute(Qt::WA_TranslucentBackground);//window设置为透明
    //设置背景
    updataBackground();
    //按钮美化
    QFile file(":/music.qss");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString str = in.readAll();
    file.close();
    this->setStyleSheet(str);
    //功能
    listmodel = new QStringListModel(this);
    list<<"在线搜索"<<"我的喜欢"<<"最近播放";
    listmodel->setStringList(list);
    ui->listView->setModel(listmodel);
    //设置playeer
    player = GlobalPlayer::instance()->player();
    playlist = GlobalPlayer::instance()->playlist();
    player->setPlaylist(playlist);
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(my_durationChanged(qint64)));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(my_positionChanged(qint64)));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onPlayerStateChanged(QMediaPlayer::State)));
    //
    search = new Search(mainWidow,ui->frame);
    search->hide();
    mylove = new Mylove(mainWidow,ui->frame);
    mylove->hide();
    recent = new Recent_play(mainWidow,ui->frame);
    recent->hide();
    // 初始化音量滑块
    ui->horizontalSlider_4->setRange(0, 200); // 设置音量滑块的范围
    ui->horizontalSlider_4->setValue(100); // 设置默认音量值
    ui->horizontalSlider_4->hide();
    connect(ui->horizontalSlider_4, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);
    connect(ui->horizontalSlider_3, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    //创建播放列表标题
    QLabel *titleLable = new QLabel("播放列表");
    titleLable->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;background-color:");

    // 创建清空按钮
    QPushButton *My_Clear = new QPushButton();

    // 设置按钮的固定大小为 32x32 像素
    My_Clear->setFixedSize(32, 32);

    // 加载源图片
    QPixmap pixmap(":/tu/清空.png");

    // 缩放图片为 32x32
    QPixmap scaledPixmap = pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 设置按钮图标
    My_Clear->setIcon(QIcon(scaledPixmap));

    // 设置按钮样式
    My_Clear->setStyleSheet(
        "QPushButton {"
        "   border: none;" // 没有边框
        "   background-color: transparent;" // 按钮的默认背景透明
        "   border-radius: 5px;" // 按钮的圆角
        "   padding: 0px;" // 设置内边距为 0，以适应按钮大小
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(0, 0, 0, 0.1);" // 鼠标悬停时的背景颜色
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(0, 0, 0, 0.2);" // 按压时的背景颜色
        "   border: 1px solid rgba(0, 0, 0, 0.3);" // 按压时的边框
        "   padding: 0px;" // 按压时的内边距
        "}"
    );
    connect(My_Clear, &QPushButton::clicked, this, &Select::on_click_Myclear);
    // 创建播放列表标题
    QLabel *titleLabel = new QLabel("播放列表", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px;");

    // 创建 QListWidget 代替 QStringListModel
    m_liebiao = new QListWidget(this);
    m_liebiao->setFocusPolicy(Qt::NoFocus);  // 不获取焦点
    connect(m_liebiao, &QListWidget::itemDoubleClicked, this, &Select::onPlaylistItemClicked);

    //水平布局
    QHBoxLayout *horLayout = new QHBoxLayout();//水平布局
    horLayout->addWidget(titleLabel);
    horLayout->addWidget(My_Clear);
    // 创建垂直布局
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(horLayout);
    layout->addWidget(m_liebiao); // 将 QListWidget 添加到布局
    layout->setSpacing(0);
    // QWidget 用作容器来存放布局
    widget = new QWidget(this);
    widget->setLayout(layout);

    // 设置播放列表的位置和大小
    widget->setGeometry(700, 100, 400, 500);
    widget->hide(); // 默认不显示

    m_liebiao->setStyleSheet(
        "QListWidget {"
        "   background-color: rgba(255, 255, 255, 0.9);"
        "   border: 1px solid rgba(0, 0, 0, 0.1);"
        "   border-radius: 10px;"
        "   padding: 5px;"
        "}"
        "QListWidget::item {"
        "   padding: 10px;"  // 增加项的内边距
        "   font-size: 14px;" // 设置字体大小
        "   color: #333333;"  // 设置字体颜色
        "}"
        "QListWidget::item:selected {"
        "   background-color: rgba(255, 140, 0, 0.3);" // 选中项的背景颜色
        "   color: white;" // 选中项的字体颜色
        "}"
    );

    // 设置字体
    QFont font("PingFang SC", 14); // 设置为更接近网易云的字体和大小
    m_liebiao->setFont(font);
}

Select::~Select()
{
    delete ui;
}

void Select::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updataBackground();
}

void Select::updataBackground()
{
    // 加载背景图片
    QPixmap background(":/tu/b1.png");

    // 检查图片是否成功加载
    if (background.isNull()) {
        qDebug() << "Failed to load background image: :/tu/back.jpg";
        return;
    }

    // 确保图片不模糊
    background = background.scaled(ui->widget_3->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // 创建一个白色背景的QPixmap
    QPixmap whiteBackground(background.size());
    whiteBackground.fill(Qt::white);

    // 将背景图片绘制到白色背景上
    QPainter painter(&whiteBackground);
    painter.drawPixmap(0, 0, background);

    // 创建一个带有弧度的矩形路径
    QPainterPath path;
    QRectF rect(0, 0, ui->widget_3->width(), ui->widget_3->height());
    qreal radius = 20; // 弧度半径
    path.addRoundedRect(rect, radius, radius);

    // 创建一个带有弧度的背景图片
    QPixmap roundedBackground(background.size());
    roundedBackground.fill(Qt::transparent);
    QPainter roundedPainter(&roundedBackground);
    roundedPainter.setRenderHint(QPainter::Antialiasing);
    roundedPainter.setClipPath(path);
    roundedPainter.drawPixmap(0, 0, whiteBackground);

    // 设置调色板
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(roundedBackground));
    ui->widget_3->setPalette(palette);

    // 设置窗口背景透明
    ui->widget_3->setAutoFillBackground(true);
}


void Select::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mouse_pos = event->globalPos();
        window_pos = this->pos();
        diff_pos = mouse_pos - window_pos;
        event->accept();
    }
}

void Select::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        QPoint pos = event->globalPos();
        this->move(pos - diff_pos);
        event->accept();
    }
}

void Select::revolve()
{

}

void Select::show_SongName()
{
    int currentIndex = playlist->currentIndex(); // 获取当前播放的索引
    if (currentIndex >= 0 && currentIndex < mainWidow->song_List.size()) {
        MainWindow::song currentMainSong = mainWidow->song_List[currentIndex]; // 获取当前播放的歌曲信息
        qDebug() << "现在播放的歌名：" << currentMainSong.songname << "时长为：" << currentMainSong.Time; // 输出歌曲名

        // 插入 QLabel 空间显示歌曲名和歌手
        QString songInfo = QString("%1-%2") // 将歌曲名称和歌手组合成一个字符串
                .arg(currentMainSong.songname)
                .arg(currentMainSong.singer);

        ui->label_3->setText(songInfo); // 更新 QLabel 显示的内容

        // 设置样式
        ui->label_3->setStyleSheet("font-size: 15px; "
                                   "font-weight: bold; "
                                   "padding-top: 20px; " // 内边距只在顶部设置，以将文本推至顶部
                                   );

        ui->label_3->setAlignment(Qt::AlignTop | Qt::AlignHCenter); // 设置文本顶部对齐并水平居中
    } else {
        qDebug() << "没有这首歌";
    }
}

void Select::onPlayerStateChanged(QMediaPlayer::State state)//播放时图标改变
{
    if (state == QMediaPlayer::PlayingState)
    {
        revolve();
        // 设置按钮的图片为暂停图标
        ui->pushButton_11->setStyleSheet("QPushButton {  border-image: url(:/tu/暂停-红色.png); }");
    }
    else if (state == QMediaPlayer::PausedState || state == QMediaPlayer::StoppedState)
    {
        // 设置按钮的图片为播放图标
        ui->pushButton_11->setStyleSheet("QPushButton {  border-image: url(:/tu/开始-红色.png); }");
    }
}

void Select::my_durationChanged(qint64 duration)
{
    QString formation = search->gettotaltime(duration);
    search->getCurrentsong();
    show_SongName();
    qDebug()<<"这首歌的时长为："<<formation;
    ui->label_6->setText(formation);
    ui->horizontalSlider_3->setRange(0,duration);
    highlightCurrentPlayingSong();
}

void Select::my_positionChanged(qint64 position)
{
    QString formation = search-> gettotaltime(position);
    //qDebug()<<"时长："<<formation;
    ui->label_5->setText(formation);
    ui->horizontalSlider_3->setValue(position);

}

void Select::on_pushButton_6_clicked()//关闭
{
    close();
}

void Select::on_pushButton_7_clicked()//最小化
{

    showMinimized();
}

void Select::on_pushButton_8_clicked()//最大化
{
    showMaximized();
}

void Select::on_listView_clicked(const QModelIndex &index)//选择功能
{
    QVariant ret = listmodel->data(index);
    search->hide();
    mylove->hide();
    recent->hide();
    if(ret == "在线搜索")
    {
        search->show();
    }
    else if(ret == "我的喜欢")
    {
        mylove->show();
        mylove->send_singal();
        mylove->showMy_love();
        mainWidow->love_list.clear();
    }
    else if(ret == "最近播放")
    {
        recent->show();
        recent->showTable();
    }
}

void Select::on_pushButton_11_clicked()//播放，暂停按钮
{
    if(player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();
    }
    else
    {
        player->play();
    }
}

void Select::on_pushButton_10_clicked()//上一曲
{
    playlist->previous();
    player->play();
}

void Select::on_pushButton_12_clicked()//下一曲
{
    playlist->next();
    player->play();
}

void Select::on_pushButton_14_clicked()
{
    if (ui->horizontalSlider_4->isHidden()) {
        ui->horizontalSlider_4->show();
    } else {
        ui->horizontalSlider_4->hide();
    }
}

void Select::on_pushButton_pifu_clicked()//切换皮肤
{

}

void Select::on_horizontalSlider_3_sliderMoved(int position)
{
    player->setPosition(position);
}

void Select::on_pushButton_13_clicked()//切换播放模式
{
    // 切换播放模式
    switch (currentPlaybackMode) {
    case QMediaPlaylist::Sequential:
        currentPlaybackMode = QMediaPlaylist::Loop;
        ui->pushButton_13->setStyleSheet("QPushButton {  border-image: url(:/tu/循环播放.png); }");
        // ui->pushButton_5->setText("循环");
        break;
    case QMediaPlaylist::Loop:
        currentPlaybackMode = QMediaPlaylist::CurrentItemInLoop;
        ui->pushButton_13->setStyleSheet("QPushButton {  border-image: url(:/tu/单曲循环.png); }");
        //ui->pushButton_5->setText("单曲循环");
        break;
    case QMediaPlaylist::CurrentItemInLoop:
        currentPlaybackMode = QMediaPlaylist::Random;
        ui->pushButton_13->setStyleSheet("QPushButton {  border-image: url(:/tu/随机播放.png); }");
        //ui->pushButton_5->setText("随机");
        break;
    case QMediaPlaylist::Random:
        currentPlaybackMode = QMediaPlaylist::Sequential;
        ui->pushButton_13->setStyleSheet("QPushButton {  border-image: url(:/tu/顺序播放.png); }");
        //ui->pushButton_5->setText("顺序");
        break;
    case QMediaPlaylist::CurrentItemOnce:
        // 处理 CurrentItemOnce 模式，或者将其作为默认情况处理
        currentPlaybackMode = QMediaPlaylist::Sequential;
        ui->pushButton_13->setStyleSheet("QPushButton {  border-image: url(:/tu/顺序播放.png); }");
        //ui->pushButton_5->setText("顺序");
        break;
    }
    playlist->setPlaybackMode(currentPlaybackMode);
}

void Select::showPlaylistWithAnimation()
{
    // 设置动画
    QPropertyAnimation *geometryAnimation = new QPropertyAnimation(widget, "geometry");
    geometryAnimation->setDuration(300); // 动画时长300毫秒

    // 初始位置在窗体右边外
    QRect startRect = QRect(this->width(), 100, 400, 500);
    QRect endRect = QRect(700, 100, 400, 500); // 最终位置

    widget->setGeometry(startRect); // 设置初始位置
    widget->setWindowOpacity(0); // 初始透明度为0
    widget->show(); // 显示列表视图

    // 设置位置动画
    geometryAnimation->setStartValue(startRect);
    geometryAnimation->setEndValue(endRect);
    geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);
    geometryAnimation->start();

    // 设置透明度动画
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(widget, "windowOpacity");
    opacityAnimation->setDuration(300);
    opacityAnimation->setStartValue(0); // 从完全透明开始
    opacityAnimation->setEndValue(1); // 最后变为不透明
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Select::hidePlaylistWithAnimation()
{
    // 设置动画
    QPropertyAnimation *geometryAnimation = new QPropertyAnimation(widget, "geometry");
    geometryAnimation->setDuration(300);

    QRect startRect = widget->geometry();
    QRect endRect = QRect(720, 100, 400, 500); // 隐藏的右侧位置

    // 设置隐藏动画
    geometryAnimation->setStartValue(startRect);
    geometryAnimation->setEndValue(endRect);
    geometryAnimation->setEasingCurve(QEasingCurve::InCubic); // 消失效果
    geometryAnimation->start();

    // 设置透明度动画
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(widget, "windowOpacity");
    opacityAnimation->setDuration(300);
    opacityAnimation->setStartValue(1); // 从不透明开始
    opacityAnimation->setEndValue(0); // 最后变为透明
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(opacityAnimation, &QPropertyAnimation::finished, widget, &QWidget::hide); // 动画结束后隐藏
}


void Select::loadPlaylistFromSongs() {
    m_liebiao->clear(); // 清空现有的列表项
    QList<MainWindow::song> songs = mainWidow->song_List;

    // 定义字体
    QFont font("PingFang SC", 14);

    for (const MainWindow::song& currentSong : songs) {
        QString songInfo = QString("%1 - %2 [%3]")
                .arg(currentSong.songname)
                .arg(currentSong.singer)
                .arg(search->gettotaltime(currentSong.Time*1000)); // 获取时间
        QListWidgetItem *item = new QListWidgetItem(songInfo);

        // 设置项的字体
        item->setFont(font);

        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter); // 左对齐及垂直居中
        m_liebiao->addItem(item); // 将项添加到 QListWidget
    }
}

void Select::on_pushButton_2_clicked() // 播放列表
{
    loadPlaylistFromSongs();
    if (widget->isHidden()) {
        showPlaylistWithAnimation(); // 使用动画显示播放列表
    } else {
        hidePlaylistWithAnimation(); // 使用动画隐藏播放列表
    }
}

void Select::onPlaylistItemClicked(QListWidgetItem *item)
{
    int songIndex = m_liebiao->row(item); // 获取点击项在 QListWidget 中的索引
    playlist->setCurrentIndex(songIndex); // 设置当前播放
    player->play(); // 开始播放
    highlightCurrentPlayingSong();
}

void Select::on_click_Myclear()
{
    mainWidow->song_List.clear();
    playlist->clear();

}

void Select::highlightCurrentPlayingSong() {
    // 获取当前播放的歌曲索引
    int currentIndex = playlist->currentIndex();
    qDebug()<<currentIndex;
    // 清除所有项的选中状态和背景样式
    for (int i = 0; i < m_liebiao->count(); ++i) {
        QListWidgetItem *item = m_liebiao->item(i);
        item->setSelected(false); // 清除选中状态

        // 恢复默认颜色
        item->setBackgroundColor(Qt::white); // 默认背景颜色
        item->setFont(QFont("PingFang SC", 14)); // 恢复默认字体
        item->setForeground(QBrush(Qt::black)); // 恢复默认字体颜色
    }

    // 如果当前索引有效，设置高亮项
    if (currentIndex >= 0 && currentIndex < m_liebiao->count()) {
        QListWidgetItem *currentItem = m_liebiao->item(currentIndex);
        currentItem->setSelected(true); // 选中当前正在播放的项

        // 改变正在播放的项的样式
        currentItem->setBackgroundColor(QColor(255, 140, 0)); // 设置高亮背景颜色
        currentItem->setForeground(QBrush(Qt::white)); // 设置高亮字体颜色
    }
}

