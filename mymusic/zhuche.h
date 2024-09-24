#ifndef ZHUCHE_H
#define ZHUCHE_H

#include <QWidget>
#include"comment.h"
#include<QDebug>
#include<QFile>
#include<QMessageBox>
#include <QGraphicsDropShadowEffect>
class MainWindow;
namespace Ui {
class Zhuche;
}

class Zhuche : public QWidget
{
    Q_OBJECT

public:
    explicit Zhuche(MainWindow *mainWidow,QWidget *parent = nullptr);
    ~Zhuche();
    //边框拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Zhuche *ui;
    QPoint diff_pos;  // 鼠标和窗口的相对位移
    QPoint window_pos;
    QPoint mouse_pos;
    MainWindow *mainWidow;
};

#endif // ZHUCHE_H
