#include "zhuche.h"
#include "ui_zhuche.h"
#include"mainwindow.h"
Zhuche::Zhuche(MainWindow *mainWidow,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Zhuche),mainWidow(mainWidow)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);//window设置为透明
    QFile file2(":/zhuche.qss");
    file2.open(QIODevice::ReadOnly);
    QTextStream in(&file2);
    in.setCodec("UTF-8");
    QString str = in.readAll();
    file2.close();
    this->setStyleSheet(str);
}

Zhuche::~Zhuche()
{
    delete ui;
}

void Zhuche::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mouse_pos = event->globalPos();
        window_pos = this->pos();
        diff_pos = mouse_pos - window_pos;
        event->accept();
    }
}

void Zhuche::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton) {
        QPoint pos = event->globalPos();
        this->move(pos - diff_pos);
        event->accept();
    }
}

void Zhuche::on_pushButton_2_clicked()//关闭
{
    close();
}

void Zhuche::on_pushButton_3_clicked()//最小化
{
    showMinimized();
}

void Zhuche::on_pushButton_clicked()//注册按钮
{
    QString user_name = ui->lineEdit->text();
    QString user_pd = ui->lineEdit_2->text();
    QString user_phone = ui->lineEdit_3->text();

    if (user_name.isEmpty() || user_pd.isEmpty() || user_phone.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "所有字段都不能为空，请填写完整信息。");
        return;
    }
    if(ui->radioButton->isChecked())
    {
        qDebug()<<"已选";
    }
    else
    {
        QMessageBox::warning(this, "输入错误", "请先阅读相关协议");
        return;
    }

    Qmesg msg;
    msg.flag = 1;
    std::string str1 = user_name.toStdString();
    std::string str2 = user_pd.toStdString();
    std::string str3 = user_phone.toStdString();

    // 定义一个注册结构体
    user_text user(str1.c_str(), str2.c_str(), str3.c_str());
    memcpy(msg.data, &user, sizeof(user));
    qint64 bytesWritten = mainWidow->clientSocket->write((char *)&msg, sizeof(msg));
    if (bytesWritten == -1) {
        // 处理写入错误
        qDebug() << "Failed to write data to socket.";
    }
}

void Zhuche::on_pushButton_4_clicked()//协议
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("隐私协议");
    msgBox.setTextFormat(Qt::RichText); // 允许使用富文本
    msgBox.setText(
        "我们非常重视您的隐私，并致力于保护您的个人信息。请仔细阅读以下内容：<br><br>"
        "1. 信息收集：我们收集您的注册信息、使用信息和设备信息。<br>"
        "2. 信息使用：我们使用您的信息来提供、维护和改进我们的服务。<br>"
        "3. 信息共享：我们不会出售您的信息，仅在必要时与服务提供商和法律要求共享。<br>"
        "4. 信息保护：我们采取合理的安全措施来保护您的个人信息。<br>"
        "5. 您的权利：您有权访问、更正或删除您的个人信息。<br>"
        "6. 隐私政策的变更：我们可能会更新本隐私协议，并在网站上发布更新。<br><br>"
        "如果您有任何疑问，请联系我们：<br>"
        "联系邮箱：privacy@example.com<br>"
        "联系电话：+1-123-456-7890<br><br>"
        "请点击“同意”按钮以表示您已阅读并同意本隐私协议的条款。"
    );
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}
