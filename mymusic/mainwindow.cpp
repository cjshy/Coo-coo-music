#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //图标设置
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint);//主窗口边框去掉
    //窗体透明
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent); // 确保绘制事件有效
    ui->centralwidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->centralwidget->setStyleSheet("background: transparent;");
    //
    ui->label->setStyleSheet("border-image: url(:/tu/zw.png);border:1px solid black; border-radius: 50px;");
    //账号文本框
    QAction *searchAction = new QAction(ui->LineEdit);
    searchAction->setIcon(QIcon(":/tu/login_User.png"));
    ui->LineEdit->addAction(searchAction,QLineEdit::LeadingPosition);
    //密码文本框
    QAction *searchAction_2 = new QAction(ui->LineEdit);
    searchAction_2->setIcon(QIcon(":/tu/login_Password.png"));
    ui->LineEdit_2->addAction(searchAction_2,QLineEdit::LeadingPosition);
    QAction *searchAction_3 = new QAction(ui->LineEdit);
    searchAction_3->setIcon(QIcon(":/tu/233键盘-线性.png"));
    ui->LineEdit_2->addAction(searchAction_3,QLineEdit::TrailingPosition);
    //LOG
    QPixmap pixmap_log(":/tu/contact.png");
    pixmap_log = pixmap_log.scaled(40,32,Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_3->setPixmap(pixmap_log);

    m_movie = new QMovie(":/tu/backg.gif");
    ui->label_2->setMovie(m_movie);
    m_movie->start();
    //按钮美化
    QFile file(":/login.qss");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString str = in.readAll();
    file.close();
    this->setStyleSheet(str);
    //
    select = new Select(this);
    select->hide();
    zhuche = new Zhuche(this);
    zhuche->hide();
    //网络连接
    clientSocket = new QTcpSocket;//创建套接4字
    clientSocket->connectToHost("192.168.10.194",9000);//连接服务器
    connect(clientSocket,SIGNAL(connected()),this,SLOT(clientConnect()));
    connect(clientSocket,SIGNAL(readyRead()),this,SLOT(Readyread()));
    //创建本地数据库，存放最近播放数据
    sql = new sqltext;
    sql->openDb();
    //初始化系统托盘
    systemtrayicon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/tu/contact.png");
    //添加图标
    systemtrayicon->setIcon(icon);
    //当鼠标悬浮时显示文字
    systemtrayicon->setToolTip(QObject::tr("咕咕音乐"));
    //显示图标
    systemtrayicon->show();
    //设置系统托盘菜单项
    menu = new QMenu(this);
    m_pShowAction = new QAction("打开主界面");
    m_pCloseAction = new QAction("退出");
    menu->addAction(m_pShowAction);
    menu->addSeparator();
    menu->addAction(m_pCloseAction);
    systemtrayicon->setContextMenu(menu);
    connect(m_pShowAction,SIGNAL(triggered(bool)),this,SLOT(showWidget()));
    connect(m_pCloseAction,SIGNAL(triggered(bool)),this,SLOT(on_pushButton_3_clicked()));
//    connect(systemtrayicon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(showWidget()));
    //账号下拉框
    //获取运行目录
    QString fileName = QCoreApplication::applicationDirPath()+"//..//UserFile";
    infoList = GetDirNameList(fileName);
    //加载自定义下拉列表
    m_AccountList = new QListWidget(this);
    m_AccountList->setFocusPolicy(Qt::NoFocus);
    ui->comboBox->setModel(m_AccountList->model());
    ui->comboBox->setView(m_AccountList);
    int infosize =0;
    myMapper = new QSignalMapper(this);

    for(int i=0;i<infoList.size();i++)
    {
        //获取账号
        QString local_account = infoList.at(i);
        QString local_name;//获取昵称
        QString local_passwd;//获取密码
        QFile file_my(fileName+QString("//%1//data.txt").arg(infoList.at(i)));
        if(!file_my.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            qDebug()<<"文件打开失败";
        }
        this->ispasswd = false;
        while(!file_my.atEnd())
        {
            infopasswd.append("");
            QByteArray line = file_my.readLine();
            QString str(QString::fromLocal8Bit(line.data()));
            if(this->ispasswd)
            {
                infopasswd.insert(i,str);
//                qDebug()<<"有密码";
                local_passwd.append(str);
                this->ispasswd = false;
            }
            else
            {
                local_name.append(str);
                this->ispasswd = true;
            }
//            qDebug()<<infopasswd;
        }
        QHBoxLayout *horLayout = new QHBoxLayout();//水平布局
        QLabel *la2 = new QLabel(QString("%1\n%2").arg(local_name.left(local_name.length()-1),local_account));
        QPushButton *b1 = new QPushButton();
        b1->setFixedSize(32,32);
        b1->setStyleSheet("QPushButton{background:rgba(200,200,200,0);border-style:solid;border-image: url(:/tu/delete.png);}"
                          "QPushButton:hover{background:rgba(200,200,200,0);border-style:solid;border-image: url(:/tu/delete2.png);}");
        connect(b1,SIGNAL(clicked(bool)),myMapper,SLOT(map()));
        myMapper->setMapping(b1,infosize);
        infoListsign.append(infosize);
        //qDebug()<<"按钮值为："<<infosize;
        infosize++;
        horLayout->addWidget(la2);
        horLayout->addWidget(b1);
        QWidget *widget = new QWidget(this);
        widget->setLayout(horLayout);
        QListWidgetItem *Listitem = new QListWidgetItem(m_AccountList);
        m_AccountList->setItemWidget(Listitem,widget);
    }
    connect(myMapper,SIGNAL(mapped(int)),this,SLOT(deleteaccount(int)));
}

MainWindow::~MainWindow()
{
    sql->closeDb();
    delete ui;

}

void MainWindow::deleteaccount(int i)
{
    QMessageBox message(QMessageBox::Question, tr("提示"), tr("是否要删除该账号信息"), QMessageBox::Yes | QMessageBox::No, this);
    int result = message.exec();

    if (result == QMessageBox::Yes) {
        int indexToRemove = infoListsign.indexOf(i);
        if (indexToRemove == -1) return; // 确保索引有效

        // 删除账号和密码
        infoList.removeAt(indexToRemove);
        infopasswd.removeAt(indexToRemove);

        // 更新界面
        if (indexToRemove == ui->comboBox->currentIndex()) {
            if (!infoList.isEmpty()) {
                ui->comboBox->setCurrentIndex(0);
            }
        }

        if (infoList.isEmpty()) {
            ui->LineEdit_2->clear();
            ui->LineEdit->clear();
            this->m_AccountList->setItemHidden(this->m_AccountList->item(0), true);
            return;
        }

        // 删除列表项
        QListWidgetItem *item = this->m_AccountList->takeItem(indexToRemove);
        if (item) {
            delete item; // 直接删除 item
        }

        infoListsign.erase(infoListsign.begin() + indexToRemove);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mouse_pos = event->globalPos();
        window_pos = this->pos();
        diff_pos = mouse_pos - window_pos;
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        QPoint pos = event->globalPos();
        this->move(pos - diff_pos);
        event->accept();
    }
}

void MainWindow::processReceivedData(const char *data, qint64 size)
{
    Qmesg *msg = (Qmesg *)data;
    qDebug() << "共读取" << size << "个字节" << data;
    if (strcmp(BUF, "登录失败") == 0) {
        QMessageBox msgBox;
        msgBox.setText("登录失败");
        msgBox.setInformativeText("用户名或密码错误");
        msgBox.exec();
        return;
    }
    else if(strcmp(BUF, "登录成功") == 0)
    {
        select->show();
        clientSocket->waitForReadyRead(1000);

        memset(BUF, 0, sizeof(BUF));
        clientSocket->read(BUF, sizeof(BUF));
        Qmesg *msg_2 = (Qmesg *)BUF;
        user_text *user = (user_text *)msg_2->data;
        strcpy(curNameid, user->Account);
        this->hide();
        qDebug()<<"登陆成功";
        //更新本地文件
        QString account = ui->LineEdit->text(); //账户
        QString name =QString(user->name);
        User_file(account,name);
        //发送信号
        Qmesg msg;
        msg.flag = 5; // 我的喜欢列表
        song_text Song(curNameid, "NULL");
        memcpy(msg.data, &Song, sizeof(song_text));
        clientSocket->write((char *)&msg, sizeof(Qmesg));
        //根据用户名创建播放数据表
        createUserPlaybackTable(curNameid);
    }
    else if (strcmp(BUF, "注册成功") == 0) {
        zhuche->close();
        clientSocket->waitForReadyRead(5000);
        memset(BUF, 0, sizeof(BUF));
        clientSocket->read(BUF, sizeof(BUF));
        QMessageBox msgBox;
        msgBox.setText("您生成的账号是：");
        msgBox.setInformativeText(BUF);
        msgBox.exec();
        return;
    }
    else if (strcmp(BUF, "注册失败") == 0) {
        QMessageBox msgBox;
        msgBox.setText("注册失败");
        msgBox.setInformativeText("手机号已被注册");
        msgBox.exec();
        return;
    }
    else if (msg->flag == 51) {
        song_text *song = (song_text *)msg->data;
        QString debugInfo = QString("用户ID: %1, 歌曲名: %2, 歌手: %3,专辑：%4 ,时间: %5, 歌曲ID: %6, 用户名: %7")
                .arg(song->user_id)
                .arg(song->music_name)
                .arg(song->singer)
                .arg(song->zhuanji)
                .arg(song->time)
                .arg(song->music_id)
                .arg(song->user_name);
        qDebug() << debugInfo;
        // 将接收到的数据存入QList<song_love> love_list中
        song_love love;
        strncpy(love.user_id, song->user_id, sizeof(love.user_id) - 1);
        strncpy(love.song_name, song->music_name, sizeof(love.song_name) - 1);
        strncpy(love.singer, song->singer, sizeof(love.singer) - 1);
        strncpy(love.time, song->time, sizeof(love.time) - 1);
        strncpy(love.song_id, song->music_id, sizeof(love.song_name) - 1);
        strncpy(love.user_name, song->user_name, sizeof(love.user_name) - 1);
        strncpy(love.album,song->zhuanji,sizeof(love.album)-1);
        love_list.append(love);
    }

    lastDisplayedCount = love_list.size();
}

void MainWindow::createUserPlaybackTable(const char *userId)//创建用户最近播放表
{
    // 根据用户ID生成表名
    QString tableName = QString("playback_%1").arg(userId);

    // 检查表是否已存在
    if (!sql->isTableExists(tableName)) {
        sql->runsql(QString("create table %1(song_name varchar(128), album varchar(128), singer varchar(128), Time varchar(128), song_id varchar(128))").arg(tableName));
        qDebug() << "创建表成功: " << tableName;
    } else {
        qDebug() << "表已存在: " << tableName;
    }

}

int MainWindow::convertDurationToSeconds(const QString &duration)
{
    QStringList timeParts = duration.split(':'); // 根据 ':' 分割
    int totalSeconds = 0;

    if (timeParts.size() == 2) { // 检查时间格式是否正确
        bool ok1, ok2;
        int minutes = timeParts[0].toInt(&ok1); // 分钟
        int seconds = timeParts[1].toInt(&ok2); // 秒

        if (ok1 && ok2) {
            // 计算总秒数
            totalSeconds = minutes * 60 + seconds;
        } else {
            qDebug() << "转换时长失败:" << duration;
        }
    } else {
        qDebug() << "时长格式不正确:" << duration;
    }

    return totalSeconds; // 返回总秒数
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    QPixmap pixmap(":/tu/login_background.png");//做好的图
    qDrawBorderPixmap(&painter, this->rect(), QMargins(0, 0, 0, 0), pixmap);
    //       QRect rect(this->centralWidget()->rect().x()+8, this->centralWidget()->rect().y()+8, this->centralWidget()->rect().width()-16, this->centralWidget()->rect().height()-16);
    //       painter.fillRect(rect, QColor(255, 255, 255));
}

void MainWindow::User_file(QString account,QString name)//更新登录信息本地文件
{
    QString passwd ="";
    if(ui->checkBox_2->isChecked())//选中记住密码
    {
        passwd = ui->LineEdit_2->text();
    }
    //获取程序当前运行目录
    QString fileName = QCoreApplication::applicationDirPath();
    //用户目录
    QString add = "//..//UserFile";
    //创建用户文件夹
    fileName = fileName +add+QString("//%1").arg(account);
    //信息保存
    QDir *file = new QDir;
    //文件夹是否已存在
    bool exist_1 =file->exists(fileName);
    if(exist_1)
    {
        QFile file(fileName+"//data.txt");
        qDebug()<<fileName+"//data.txt";
        if(file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
        {
            //            qDebug<<"文件夹创建成功";
        }
        QTextStream stream(&file);
        //写入
        if(passwd == "") stream<<name<<"\n";
        else stream<<name<<"\n"<<passwd;
        file.close();
    }
    else
    {
        bool ok = file->mkpath(fileName);
        if(ok)
        {
            qDebug()<<"创建";
            QFile file(fileName+"//data.txt");
            qDebug()<<fileName+"//data.txt";
            if(file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
            {
                qDebug()<<"文件夹创建成功";
            }
            QTextStream stream(&file);
            //写入
            if(passwd == "") stream<<name<<"\n";
            else stream<<name<<"\n"<<passwd;
            file.close();
        }
        else
        {
            qDebug()<<"未创建成功";
        }
    }
}

QStringList MainWindow::GetDirNameList(const QString &strDirpath)//用于获取用户文件下所有账号
{
    QDir dir(strDirpath);
    QFileInfoList Info_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> Iterator(Info_list);
    QStringList strDirNameList;
    QFileInfo Info;
    while (Iterator.hasNext())
    {
        Info = Iterator.next();
        if (Info.isDir())
        {
            strDirNameList << QDir(Info.absoluteFilePath()).dirName();
        }
    }
    return strDirNameList;
}

void MainWindow::clientConnect()
{
    qDebug()<<"连接服务器成功";
}

void MainWindow::Readyread()
{
    while (clientSocket->bytesAvailable() > 0) {
        memset(BUF, 0, sizeof(BUF));
        qint64 ret = clientSocket->read(BUF, sizeof(BUF));

        if (ret < 0) {
            // 处理读取错误
            qDebug() << "读取错误:" << clientSocket->errorString();
            return;
        }

        // 处理数据
        processReceivedData(BUF, ret);
    }
}

void MainWindow::on_pushButton_login_clicked()//登录
{
    Qmesg msg;
    msg.flag =2;
    QString user_id = ui->LineEdit->text();
    std::string str1 = user_id.toStdString();

    QString user_pd =ui->LineEdit_2->text();
    std::string str2 = user_pd.toStdString();

    //定义一个登录结构体
    user_text user(str1.c_str(),str2.c_str());
    memcpy(msg.data,&user,sizeof(user));
    clientSocket->write((char *)&msg,sizeof(msg));
}

void MainWindow::on_pushButton_4_clicked()//注册
{
    zhuche->show();
}

void MainWindow::on_pushButton_3_clicked()//关闭
{
    this-> close();
}

void MainWindow::on_pushButton_5_clicked()//最小化
{
    this->hide();
    //showMinimized();
}

void MainWindow::showWidget()//显示图像
{
    this->show();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    ui->checkBox_2->setChecked(false);
    ui->LineEdit_2->setText("");
    ui->LineEdit->setText(infoList.at(index));
    if(infopasswd.at(index)!="")
    {
        ui->LineEdit_2->setText(infopasswd.at(index));
        ui->checkBox_2->setChecked(true);
    }

}
