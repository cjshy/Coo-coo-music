#include "customdelegete.h"

customdelegete::customdelegete()
{

}

void customdelegete::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // QStyledItemDelegate::paint(painter, option, index);
    QVariant ret = index.model()->data(index);
    //qDebug() << ret;
    QImage image;
    if (ret == "未收藏")
    {
        image.load(":/tu/爱心.png");
    }
    else if (ret == "已收藏")
    {
        image.load(":/tu/爱心 (1).png");
    }

    //qDebug() << option.rect;
    QRect rect = option.rect;

    // 计算图片的缩放比例
    double scale = qMin(static_cast<double>(rect.width()) / image.width(), static_cast<double>(rect.height()) / image.height());
    int newWidth = static_cast<int>(image.width() * scale);
    int newHeight = static_cast<int>(image.height() * scale);

    // 计算图片的居中位置
    int x = rect.left() + (rect.width() - newWidth) / 2;
    int y = rect.top() + (rect.height() - newHeight) / 2;

    // 绘制图片
    painter->drawImage(QRect(x, y, newWidth, newHeight), image);
}
