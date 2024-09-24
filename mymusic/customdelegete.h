#ifndef CUSTOMDELEGETE_H
#define CUSTOMDELEGETE_H
#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>
#include <QApplication>

class customdelegete:public QStyledItemDelegate
{
public:
    customdelegete();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CUSTOMDELEGETE_H
