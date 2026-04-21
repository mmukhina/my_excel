#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QMap>

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit Delegate(QObject *parent = nullptr);

    // Переопределенные методы QStyledItemDelegate
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    // Обновляет все формулы в таблице
    void updateAllFormulas(QAbstractItemModel *model) const;

private:
    mutable QMap<QString, QString> m_formulas;  // Хранение формул

    // Вычисляет значение формулы
    QString evaluateFormula(const QString &formula, const QAbstractItemModel *model, const QModelIndex &currentIndex) const;

    // Проверяет, содержит ли текст ошибку
    bool isError(const QString &text) const;

    // Проверяет, есть ли формула в указанной ячейке
    bool hasFormula(const QModelIndex &index) const;

    // Вычисляет сумму чисел в диапазоне
    double sumRange(const QString &range, const QAbstractItemModel *model) const;

    // Вычисляет среднее арифметическое чисел в диапазоне
    double averageRange(const QString &range, const QAbstractItemModel *model) const;

    // Вычисляет медиану чисел в диапазоне
    double medianRange(const QString &range, const QAbstractItemModel *model) const;

    // Получает все значения из диапазона
    QStringList getRangeValues(const QString &range, const QAbstractItemModel *model) const;

    // Получает значение ячейки по координатам
    QVariant getCellValue(int row, int col, const QAbstractItemModel *model) const;

    // Проверяет, является ли строка числом
    bool isNumeric(const QString &str) const;

    friend class MainWindow;  // MainWindow имеет доступ к m_formulas
};

#endif
