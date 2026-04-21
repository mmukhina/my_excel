#include "delegate.h"
#include <QLineEdit>
#include <QPainter>
#include <QJSEngine>
#include <QRegularExpression>
#include <algorithm>
#include <QAbstractItemModel>

Delegate::Delegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

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
    else if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color());
    } else {
        painter->fillRect(option.rect, option.palette.base());
        painter->setPen(option.palette.text().color());
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

void Delegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                            const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    QString text = lineEdit->text();
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());

    if (text.startsWith('=')) {
        m_formulas[key] = text;
        QString result = evaluateFormula(text.mid(1), model, index);
        model->setData(index, result);
    } else {
        m_formulas.remove(key);
        model->setData(index, text);
    }

    // После изменения ячейки обновляем все формулы
    updateAllFormulas(model);
}

void Delegate::updateAllFormulas(QAbstractItemModel *model) const
{
    // Перебираем все сохраненные формулы
    for (auto it = m_formulas.begin(); it != m_formulas.end(); ++it) {
        QString key = it.key();
        QString formula = it.value();

        // Разбираем ключ "row:column"
        QStringList parts = key.split(':');
        if (parts.size() == 2) {
            int row = parts[0].toInt();
            int col = parts[1].toInt();

            if (row < model->rowCount() && col < model->columnCount()) {
                QModelIndex idx = model->index(row, col);
                // Пересчитываем формулу
                QString result = evaluateFormula(formula.mid(1), model, idx);
                model->setData(idx, result);
            }
        }
    }
}

bool Delegate::isError(const QString &text) const
{
    return text.contains("ERROR", Qt::CaseInsensitive) || text.contains("ОШИБКА");
}

bool Delegate::hasFormula(const QModelIndex &index) const
{
    QString key = QString("%1:%2").arg(index.row()).arg(index.column());
    return m_formulas.contains(key);
}

QString Delegate::evaluateFormula(const QString &formula, const QAbstractItemModel *model, const QModelIndex &currentIndex) const
{
    Q_UNUSED(currentIndex)

    QString expr = formula.trimmed().toUpper();

    // Проверка на функции с диапазоном
    QRegularExpression funcRegex("^(SUM|AVERAGE|MEDIAN)\\(([A-Z]+[0-9]+:[A-Z]+[0-9]+)\\)$");
    QRegularExpressionMatch match = funcRegex.match(expr);

    if (match.hasMatch()) {
        QString func = match.captured(1);
        QString range = match.captured(2);

        if (func == "SUM") return QString::number(sumRange(range, model));
        if (func == "AVERAGE") return QString::number(averageRange(range, model));
        if (func == "MEDIAN") return QString::number(medianRange(range, model));
    }

    // Проверка на функции с одной ячейкой TOLOWER и TOUPPER
    QRegularExpression singleFuncRegex("^(TOLOWER|TOUPPER)\\(([A-Z]+[0-9]+)\\)$");
    QRegularExpressionMatch singleMatch = singleFuncRegex.match(expr);

    if (singleMatch.hasMatch()) {
        QString func = singleMatch.captured(1);
        QString cell = singleMatch.captured(2);

        // Получаем значение из одной ячейки
        QRegularExpression cellRegex("([A-Z]+)([0-9]+)");
        QRegularExpressionMatch cellMatch = cellRegex.match(cell);

        if (cellMatch.hasMatch()) {
            QString colStr = cellMatch.captured(1);
            QString rowStr = cellMatch.captured(2);

            // Конвертируем букву столбца в число
            int col = 0;
            for (int i = 0; i < colStr.length(); ++i) {
                col = col * 26 + (colStr[i].toLatin1() - 'A' + 1);
            }
            col--;
            int row = rowStr.toInt() - 1;

            QVariant value = getCellValue(row, col, model);

            if (func == "TOLOWER") {
                return value.toString().toLower();
            }
            if (func == "TOUPPER") {
                return value.toString().toUpper();
            }
        }
        return "ERROR";
    }

    // Замена ссылок на ячейки их значениями
    QRegularExpression cellRegex("([A-Z]+)([0-9]+)");
    QRegularExpressionMatchIterator it = cellRegex.globalMatch(expr);
    QMap<QString, QString> replacements;

    while (it.hasNext()) {
        QRegularExpressionMatch cellMatch = it.next();
        QString colStr = cellMatch.captured(1);
        QString rowStr = cellMatch.captured(2);

        // Конвертируем букву столбца в число
        int col = 0;
        for (int i = 0; i < colStr.length(); ++i) {
            col = col * 26 + (colStr[i].toLatin1() - 'A' + 1);
        }
        col--;
        int row = rowStr.toInt() - 1;

        // Получаем значение ячейки
        QVariant value = getCellValue(row, col, model);
        QString replacement;

        if (value.isValid() && isNumeric(value.toString())) {
            replacement = value.toString();
        } else if (value.isValid()) {
            replacement = "\"" + value.toString() + "\"";
        } else {
            replacement = "0";
        }

        replacements[cellMatch.captured(0)] = replacement;
    }

    // Выполняем замены
    for (auto it = replacements.begin(); it != replacements.end(); ++it) {
        expr.replace(it.key(), it.value());
    }

    // Вычисляем выражение
    QJSEngine engine;
    QJSValue result = engine.evaluate(expr);

    if (result.isError()) {
        return "ERROR";
    }

    if (result.isNumber()) {
        double val = result.toNumber();
        if (val == int(val)) {
            return QString::number(int(val));
        }
        return QString::number(val);
    }

    return result.toString();
}

double Delegate::sumRange(const QString &range, const QAbstractItemModel *model) const
{
    QStringList values = getRangeValues(range, model);
    double sum = 0;
    for (const QString &v : values) {
        if (isNumeric(v)) sum += v.toDouble();
    }
    return sum;
}

double Delegate::averageRange(const QString &range, const QAbstractItemModel *model) const
{
    QStringList values = getRangeValues(range, model);
    double sum = 0;
    int count = 0;
    for (const QString &v : values) {
        if (isNumeric(v)) {
            sum += v.toDouble();
            count++;
        }
    }
    return count > 0 ? sum / count : 0;
}

double Delegate::medianRange(const QString &range, const QAbstractItemModel *model) const
{
    QStringList values = getRangeValues(range, model);
    QVector<double> nums;
    for (const QString &v : values) {
        if (isNumeric(v)) nums.append(v.toDouble());
    }
    if (nums.isEmpty()) return 0;
    std::sort(nums.begin(), nums.end());
    int size = nums.size();
    if (size % 2 == 0) {
        return (nums[size/2 - 1] + nums[size/2]) / 2.0;
    } else {
        return nums[size/2];
    }
}

QStringList Delegate::getRangeValues(const QString &range, const QAbstractItemModel *model) const
{
    QStringList values;
    QRegularExpression regex("^([A-Z]+)([0-9]+):([A-Z]+)([0-9]+)$");
    QRegularExpressionMatch match = regex.match(range);

    if (!match.hasMatch()) return values;

    // Получаем координаты
    QString startColStr = match.captured(1);
    int startRow = match.captured(2).toInt() - 1;
    QString endColStr = match.captured(3);
    int endRow = match.captured(4).toInt() - 1;

    // Конвертируем буквы в числа
    int startCol = 0, endCol = 0;
    for (int i = 0; i < startColStr.length(); ++i) {
        startCol = startCol * 26 + (startColStr[i].toLatin1() - 'A' + 1);
    }
    startCol--;

    for (int i = 0; i < endColStr.length(); ++i) {
        endCol = endCol * 26 + (endColStr[i].toLatin1() - 'A' + 1);
    }
    endCol--;

    // Собираем значения
    for (int row = qMin(startRow, endRow); row <= qMax(startRow, endRow); ++row) {
        for (int col = qMin(startCol, endCol); col <= qMax(startCol, endCol); ++col) {
            QVariant value = getCellValue(row, col, model);
            if (value.isValid()) {
                values.append(value.toString());
            } else {
                values.append("");
            }
        }
    }

    return values;
}

QVariant Delegate::getCellValue(int row, int col, const QAbstractItemModel *model) const
{
    if (row >= 0 && col >= 0 && row < model->rowCount() && col < model->columnCount()) {
        QModelIndex cellIndex = model->index(row, col);
        return cellIndex.data();
    }
    return QVariant();
}

bool Delegate::isNumeric(const QString &str) const
{
    if (str.isEmpty()) return false;
    bool ok;
    str.toDouble(&ok);
    return ok;
}
