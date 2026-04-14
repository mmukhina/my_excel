#ifndef COLORDELEGATE_H
#define COLORDELEGATE_H

#include <QStyledItemDelegate>

class ColorDelegate : public QStyledItemDelegate
{
public:
    explicit ColorDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

#endif
