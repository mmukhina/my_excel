#include "colordelegate.h"
#include <QPainter>
#include <QApplication>

ColorDelegate::ColorDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void ColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    // Get the text from the cell
    QString text = index.data().toString();

    // Save the current state
    painter->save();

    // If text contains "error", make it red
    if (text.contains("ERROR")) {
        painter->fillRect(option.rect, QColor(255, 200, 200)); // Light red background
        painter->setPen(Qt::red);
    }

    // Draw the text
    painter->drawText(option.rect.adjusted(5, 0, -5, 0),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      text);

    painter->restore();
}
