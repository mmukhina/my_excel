#include "formuladelegate.h"
#include <QLineEdit>
#include <QRegularExpression>
#include <QDebug>

FormulaDelegate::FormulaDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

// This creates the editor when you double-click a cell
QWidget* FormulaDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    Q_UNUSED(option)

    QLineEdit *editor = new QLineEdit(parent);

    // If this cell has a stored formula, show it in the editor
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());
    if (m_formulas.contains(key)) {
        editor->setText(m_formulas[key]);  // Show the formula (e.g., "=4+3")
    } else {
        editor->setText(index.data().toString());  // Show the displayed value
    }

    return editor;
}

// This loads data into the editor when editing starts
void FormulaDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    QString key = QString("%1:%2").arg(index.row()).arg(index.column());

    // If there's a stored formula, show it in the editor
    if (m_formulas.contains(key)) {
        lineEdit->setText(m_formulas[key]);  // Show "=4+3"
    } else {
        lineEdit->setText(index.data().toString());  // Show "7"
    }
}

// This saves data when editing is finished
void FormulaDelegate::setModelData(QWidget *editor,
                                   QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    QString text = lineEdit->text();
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());

    // THE MAGIC: Check if it starts with "="
    if (text.startsWith('=')) {
        // Store the formula
        m_formulas[key] = text;

        // Evaluate and store the result in the model
        QString result = evaluateFormula(text.mid(1));  // Remove the '='
        model->setData(index, result);

        qDebug() << "Stored formula:" << text << "→ Result:" << result;
    } else {
        // Regular text, remove any stored formula
        m_formulas.remove(key);
        model->setData(index, text);
    }
}

// Store a formula for a cell
void FormulaDelegate::setFormula(const QModelIndex &index, const QString &formula)
{
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());
    if (formula.startsWith('=')) {
        m_formulas[key] = formula;
    }
}

// Retrieve a formula for a cell (empty string if not a formula cell)
QString FormulaDelegate::getFormula(const QModelIndex &index) const
{
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());
    return m_formulas.value(key, QString());
}

// Simple formula evaluator - you can expand this!
QString FormulaDelegate::evaluateFormula(const QString &formula) const
{
    QString expr = formula.trimmed();
    expr.remove(' ');  // Remove spaces

    // Simple arithmetic: number operator number
    QRegularExpression re("^(\\d+)([+\\-*/])(\\d+)$");
    QRegularExpressionMatch match = re.match(expr);

    if (match.hasMatch()) {
        double left = match.captured(1).toDouble();
        QString op = match.captured(2);
        double right = match.captured(3).toDouble();

        if (op == "+") return QString::number(left + right);
        if (op == "-") return QString::number(left - right);
        if (op == "*") return QString::number(left * right);
        if (op == "/") {
            if (right != 0) return QString::number(left / right);
            return "ERROR: Division by zero";
        }
    }

    return "ERROR: Invalid formula";
}
