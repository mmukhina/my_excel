#include "delegate.h"
#include <QLineEdit>
#include <QPainter>
#include <QJSEngine>
#include <QQmlEngine>
#include <QApplication>

// Constructor
Delegate::Delegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

// Paint method implementation
void Delegate::paint(QPainter *painter,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    QString text = index.data().toString();

    painter->save();

    if (isError(text)) {
        painter->fillRect(option.rect, QColor(255, 200, 200));
        painter->setPen(Qt::red);
    }
    else if (hasFormula(index)) {
        painter->fillRect(option.rect, QColor(200, 220, 255));
        painter->setPen(Qt::darkBlue);
    }
    else {
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(option.palette.highlightedText().color());
        } else {
            painter->fillRect(option.rect, option.palette.base());
            painter->setPen(option.palette.text().color());
        }
    }

    painter->drawText(option.rect.adjusted(5, 0, -5, 0),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      text);

    if (hasFormula(index)) {
        painter->setPen(QColor(100, 150, 255));
        painter->drawRect(option.rect.adjusted(0, 0, -1, -1));
    }

    painter->restore();
}

// createEditor implementation
QWidget* Delegate::createEditor(QWidget *parent,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    Q_UNUSED(option)

    QLineEdit *editor = new QLineEdit(parent);

    QString key = QString("%1:%2").arg(index.row()).arg(index.column());
    if (m_formulas.contains(key)) {
        editor->setText(m_formulas[key]);
    } else {
        editor->setText(index.data().toString());
    }

    return editor;
}

// setEditorData implementation
void Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    QString key = QString("%1:%2").arg(index.row()).arg(index.column());

    if (m_formulas.contains(key)) {
        lineEdit->setText(m_formulas[key]);
    } else {
        lineEdit->setText(index.data().toString());
    }
}

// setModelData implementation
void Delegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                            const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    QString text = lineEdit->text();
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());

    if (text.startsWith('=')) {
        m_formulas[key] = text;
        QString result = evaluateFormula(text.mid(1));
        model->setData(index, result);
    } else {
        m_formulas.remove(key);
        model->setData(index, text);
    }
}

// Helper method implementations
bool Delegate::isError(const QString &text) const
{
    return text.contains("error", Qt::CaseInsensitive);
}

bool Delegate::hasFormula(const QModelIndex &index) const
{
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());
    return m_formulas.contains(key);
}

// evaluateFormula implementation
QString Delegate::evaluateFormula(const QString &formula) const
{
    QJSEngine engine;
    QJSValue result = engine.evaluate(formula);

    if (result.isError()) {
        return "ERROR: " + result.toString();
    }

    if (result.isNumber()) {
        double value = result.toNumber();
        if (qFuzzyCompare(value + 1.0, qRound(value) + 1.0)) {
            return QString::number(qRound(value));
        }
        return QString::number(value, 'g', 10);
    }

    if (result.isString()) {
        return result.toString();
    }

    return result.toString();
}
