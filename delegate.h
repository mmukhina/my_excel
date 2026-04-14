#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QMap>

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit Delegate(QObject *parent = nullptr);

    // These MUST be declared exactly as in the .cpp file
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private:
    mutable QMap<QString, QString> m_formulas;

    QString evaluateFormula(const QString &formula) const;
    bool isError(const QString &text) const;
    bool hasFormula(const QModelIndex &index) const;
};

#endif
