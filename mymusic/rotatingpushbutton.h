#ifndef ROTATINGPUSHBUTTON_H
#define ROTATINGPUSHBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QPropertyAnimation>

#include <QPushButton>
#include <QPainter>
#include <QPropertyAnimation>

class RotatingPushButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

public:
    explicit RotatingPushButton(QWidget *parent = nullptr)
        : QPushButton(parent), m_rotation(0)
    {
        // 创建旋转动画
        m_animation = new QPropertyAnimation(this, "rotation", this);
        m_animation->setDuration(2000); // 2 秒
        m_animation->setStartValue(0);
        m_animation->setEndValue(360);
        m_animation->setLoopCount(-1); // 无限循环
    }

    qreal rotation() const { return m_rotation; }
    void setRotation(qreal rotation)
    {
        m_rotation = rotation;
        update(); // 重绘按钮
    }

    void startRotation() { m_animation->start(); }
    void stopRotation() { m_animation->stop(); }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 保存当前的变换矩阵
        painter.save();

        // 应用旋转
        painter.translate(width() / 2, height() / 2);
        painter.rotate(m_rotation);
        painter.translate(-width() / 2, -height() / 2);

        // 绘制按钮内容
        QPushButton::paintEvent(event);

        // 恢复变换矩阵
        painter.restore();
    }

private:
    qreal m_rotation;
    QPropertyAnimation *m_animation;
};

#endif // ROTATINGPUSHBUTTON_H
