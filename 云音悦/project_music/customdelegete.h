#ifndef CUSTOMDELEGETE_H
#define CUSTOMDELEGETE_H
#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QItemDelegate>

class customDelegete : public QStyledItemDelegate
{
    Q_OBJECT
public:
    customDelegete();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CUSTOMDELEGETE_H
