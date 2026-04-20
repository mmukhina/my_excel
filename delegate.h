#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QMap>

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit Delegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    // Метод для обновления всех формул (теперь const)
    void updateAllFormulas(QAbstractItemModel *model) const;

private:
    mutable QMap<QString, QString> m_formulas;

    QString evaluateFormula(const QString &formula, const QAbstractItemModel *model, const QModelIndex &currentIndex) const;
    bool isError(const QString &text) const;
    bool hasFormula(const QModelIndex &index) const;

    double sumRange(const QString &range, const QAbstractItemModel *model, const QModelIndex &currentIndex) const;
    double averageRange(const QString &range, const QAbstractItemModel *model, const QModelIndex &currentIndex) const;
    double medianRange(const QString &range, const QAbstractItemModel *model, const QModelIndex &currentIndex) const;
    QString toLowerRange(const QString &range, const QAbstractItemModel *model, const QModelIndex &currentIndex) const;
    QString toUpperRange(const QString &range, const QAbstractItemModel *model, const QModelIndex &currentIndex) const;

    QStringList getRangeValues(const QString &range, const QAbstractItemModel *model) const;
    QVariant getCellValue(int row, int col, const QAbstractItemModel *model) const;
    bool isNumeric(const QString &str) const;

    friend class MainWindow;
};

#endif
