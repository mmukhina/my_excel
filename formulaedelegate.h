#ifndef FORMULADELEGATE_H
#define FORMULADELEGATE_H

#include <QStyledItemDelegate>
#include <QMap>

class FormulaDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit FormulaDelegate(QObject *parent = nullptr);

    // Override these key methods
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    // Helper methods to store/retrieve formulas
    void setFormula(const QModelIndex &index, const QString &formula);
    QString getFormula(const QModelIndex &index) const;

private:
    // Store formulas using a unique key "row:column"
    mutable QMap<QString, QString> m_formulas;

    // Evaluate a formula string (simple version)
    QString evaluateFormula(const QString &formula) const;
};

#endif
