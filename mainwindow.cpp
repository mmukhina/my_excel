#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "delegate.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Настройка минимального размера окна
    setMinimumSize(400, 300);

    // Настройка скроллинга таблицы
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // Генерация заголовков столбцов A, B, C, ... Z, AA, AB, ...
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        QString header;
        if (col < 26) {
            header = QString(QChar('A' + col));
        } else {
            int first = (col / 26) - 1;
            int second = col % 26;
            header = QString(QChar('A' + first)) + QString(QChar('A' + second));
        }
        ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(header));
    }

    // Генерация заголовков строк (1, 2, 3, ...)
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(row + 1)));
    }

    // Установка делегата для поддержки формул
    Delegate *delegate = new Delegate(this);
    ui->tableWidget->setItemDelegate(delegate);

    // Подключение сигнала изменения ячейки для обновления формул
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, [this](int row, int column) {
        Q_UNUSED(row);
        Q_UNUSED(column);
        Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());
        if (del) {
            del->updateAllFormulas(ui->tableWidget->model());
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    // Очищаем содержимое таблицы
    clearTable();

    // Очищаем сохранённые формулы в делегате
    Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());
    if (del) {
        del->m_formulas.clear();
    }

    ui->statusbar->showMessage("Создана новая таблица");
}

void MainWindow::on_actionOpenCSV_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть CSV файл", "", "CSV files (*.csv)");
    if (!filename.isEmpty()) {
        loadCSV(filename);
    }
}

void MainWindow::on_actionOpenJSON_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Открыть JSON файл", "", "JSON files (*.json)");
    if (!filename.isEmpty()) {
        loadJSON(filename);
    }
}

void MainWindow::on_actionSaveAsCSV_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить как CSV", "", "CSV files (*.csv)");
    if (!filename.isEmpty()) {
        saveCSV(filename);
    }
}

void MainWindow::on_actionSaveAsJSON_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить как JSON", "", "JSON files (*.json)");
    if (!filename.isEmpty()) {
        saveJSON(filename);
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAddRow_triggered()
{
    int currentRow = ui->tableWidget->currentRow();

    if (currentRow >= 0) {
        // Сохраняем все данные и формулы ВСЕЙ таблицы
        QMap<QString, QString> allData;
        QMap<QString, QString> allFormulas;

        Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());

        // Сохраняем все данные таблицы
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
                QTableWidgetItem* item = ui->tableWidget->item(row, col);
                if (item) {
                    QString key = QString("%1:%2").arg(row).arg(col);
                    allData[key] = item->text();
                }
            }
        }

        // Сохраняем все формулы
        if (del) {
            allFormulas = del->m_formulas;
        }

        // Очищаем всю таблицу
        ui->tableWidget->clearContents();

        // Очищаем формулы
        if (del) {
            del->m_formulas.clear();
        }

        // Вставляем новую пустую строку
        ui->tableWidget->insertRow(currentRow);

        // Восстанавливаем данные со сдвигом
        QMap<QString, QString>::const_iterator dataIt;
        for (dataIt = allData.begin(); dataIt != allData.end(); ++dataIt) {
            QString key = dataIt.key();
            QString value = dataIt.value();

            QStringList parts = key.split(':');
            if (parts.size() == 2) {
                int oldRow = parts[0].toInt();
                int col = parts[1].toInt();

                int newRow = oldRow;
                if (oldRow >= currentRow) {
                    newRow = oldRow + 1;
                }

                ui->tableWidget->setItem(newRow, col, new QTableWidgetItem(value));
            }
        }

        // Восстанавливаем формулы со сдвигом ссылок
        QMap<QString, QString>::const_iterator formulaIt;
        for (formulaIt = allFormulas.begin(); formulaIt != allFormulas.end(); ++formulaIt) {
            QString key = formulaIt.key();
            QString formula = formulaIt.value();

            QStringList parts = key.split(':');
            if (parts.size() == 2) {
                int oldRow = parts[0].toInt();
                int col = parts[1].toInt();

                int newRow = oldRow;
                if (oldRow >= currentRow) {
                    newRow = oldRow + 1;
                }

                // Обновляем ссылки в формуле
                QString updatedFormula = updateFormulaReferences(formula, currentRow, 1);

                QString newKey = QString("%1:%2").arg(newRow).arg(col);
                if (del) {
                    del->m_formulas[newKey] = updatedFormula;
                }
            }
        }

    } else {
        // Добавляем пустую строку в конец
        int newRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(newRow);
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            ui->tableWidget->setItem(newRow, col, new QTableWidgetItem(""));
        }
    }

    // Обновляем заголовки строк
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(row + 1)));
    }

    // Обновляем все формулы
    Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());
    if (del) {
        del->updateAllFormulas(ui->tableWidget->model());
    }
}

void MainWindow::on_actionDeleteRow_triggered()
{
    int currentRow = ui->tableWidget->currentRow();

    if (currentRow >= 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение удаления",
            QString("Вы действительно хотите удалить строку %1?").arg(currentRow + 1),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            // Сохраняем все данные и формулы
            QMap<QString, QString> allData;
            QMap<QString, QString> allFormulas;

            Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());

            // Сохраняем все данные таблицы (кроме удаляемой строки)
            for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                for (int col = 0; col <ui->tableWidget->columnCount(); ++col) {
                    if (row == currentRow) continue; // Пропускаем удаляемую строку

                    QTableWidgetItem* item = ui->tableWidget->item(row, col);
                    if (item) {
                        QString key = QString("%1:%2").arg(row).arg(col);
                        allData[key] = item->text();
                    }
                }
            }

            // Сохраняем все формулы (кроме удаляемой строки)
            if (del) {
                QMap<QString, QString>::const_iterator formulaIt;
                for (formulaIt = del->m_formulas.begin(); formulaIt != del->m_formulas.end(); ++formulaIt) {
                    QString key = formulaIt.key();
                    QStringList parts = key.split(':');
                    if (parts.size() == 2) {
                        int row = parts[0].toInt();
                        if (row != currentRow) {
                            allFormulas[key] = formulaIt.value();
                        }
                    }
                }
            }

            // Очищаем таблицу
            ui->tableWidget->clearContents();

            // Очищаем формулы
            if (del) {
                del->m_formulas.clear();
            }

            // Удаляем строку
            ui->tableWidget->removeRow(currentRow);

            // Восстанавливаем данные со сдвигом вверх
            QMap<QString, QString>::const_iterator dataIt;
            for (dataIt = allData.begin(); dataIt != allData.end(); ++dataIt) {
                QString key = dataIt.key();
                QString value = dataIt.value();

                QStringList parts = key.split(':');
                if (parts.size() == 2) {
                    int oldRow = parts[0].toInt();
                    int col = parts[1].toInt();

                    int newRow = oldRow;
                    if (oldRow > currentRow) {
                        newRow = oldRow - 1;
                    }

                    ui->tableWidget->setItem(newRow, col, new QTableWidgetItem(value));
                }
            }

            // Восстанавливаем формулы со сдвигом ссылок
            QMap<QString, QString>::const_iterator formulaIt;
            for (formulaIt = allFormulas.begin(); formulaIt != allFormulas.end(); ++formulaIt) {
                QString key = formulaIt.key();
                QString formula = formulaIt.value();

                QStringList parts = key.split(':');
                if (parts.size() == 2) {
                    int oldRow = parts[0].toInt();
                    int col = parts[1].toInt();

                    int newRow = oldRow;
                    if (oldRow > currentRow) {
                        newRow = oldRow - 1;
                    }

                    // Обновляем ссылки в формуле (сдвиг вверх)
                    QString updatedFormula = updateFormulaReferences(formula, currentRow, -1);

                    QString newKey = QString("%1:%2").arg(newRow).arg(col);
                    if (del) {
                        del->m_formulas[newKey] = updatedFormula;
                    }
                }
            }

            // Обновляем заголовки строк
            for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                ui->tableWidget->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(row + 1)));
            }

            // Обновляем все формулы
            if (del) {
                del->updateAllFormulas(ui->tableWidget->model());
            }
        } else {
            ui->statusbar->showMessage("Удаление строки отменено");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для удаления");
    }
}

QString MainWindow::updateFormulaReferences(const QString &formula, int insertRow, int shift)
{
    QString result = formula;

    // Регулярное выражение для поиска ссылок на ячейки (например, A1, B2, AA10)
    QRegularExpression cellRegex("([A-Z]+)([0-9]+)");
    QRegularExpressionMatchIterator regexIterator = cellRegex.globalMatch(formula);

    QMap<QString, QString> replacements;

    while (regexIterator.hasNext()) {
        QRegularExpressionMatch match = regexIterator.next();
        QString originalRef = match.captured(0);
        QString colStr = match.captured(1);
        QString rowStr = match.captured(2);

        int row = rowStr.toInt() - 1; // Переводим в 0-индексацию

        // Обновляем номер строки если нужно
        if (row >= insertRow) {
            int newRow = row + shift;
            if (newRow >= 0) {
                QString newRowStr = QString::number(newRow + 1);
                QString newRef = colStr + newRowStr;
                replacements[originalRef] = newRef;
            }
        }
    }

    // Выполняем замены
    QMap<QString, QString>::const_iterator replacementIt;
    for (replacementIt = replacements.begin(); replacementIt != replacements.end(); ++replacementIt) {
        result.replace(replacementIt.key(), replacementIt.value());
    }

    return result;
}

void MainWindow::on_actionAddColumn_triggered()
{
    int currentCol = ui->tableWidget->currentColumn();

    if (currentCol >= 0) {
        // Сохраняем все данные и формулы ВСЕЙ таблицы
        QMap<QString, QString> allData;
        QMap<QString, QString> allFormulas;

        Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());

        // Сохраняем все данные таблицы
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
                QTableWidgetItem* item = ui->tableWidget->item(row, col);
                if (item) {
                    QString key = QString("%1:%2").arg(row).arg(col);
                    allData[key] = item->text();
                }
            }
        }

        // Сохраняем все формулы
        if (del) {
            allFormulas = del->m_formulas;
        }

        // Очищаем всю таблицу
        ui->tableWidget->clearContents();

        // Очищаем формулы
        if (del) {
            del->m_formulas.clear();
        }

        // Вставляем новую пустую колонку ПОСЛЕ текущей
        ui->tableWidget->insertColumn(currentCol + 1);

        // Восстанавливаем данные со сдвигом
        QMap<QString, QString>::const_iterator dataIt;
        for (dataIt = allData.begin(); dataIt != allData.end(); ++dataIt) {
            QString key = dataIt.key();
            QString value = dataIt.value();

            QStringList parts = key.split(':');
            if (parts.size() == 2) {
                int row = parts[0].toInt();
                int oldCol = parts[1].toInt();

                int newCol = oldCol;
                // Если старая колонка была ПОСЛЕ позиции вставки, сдвигаем вправо
                if (oldCol > currentCol) {
                    newCol = oldCol + 1;
                }

                ui->tableWidget->setItem(row, newCol, new QTableWidgetItem(value));
            }
        }

        // Восстанавливаем формулы со сдвигом ссылок
        QMap<QString, QString>::const_iterator formulaIt;
        for (formulaIt = allFormulas.begin(); formulaIt != allFormulas.end(); ++formulaIt) {
            QString key = formulaIt.key();
            QString formula = formulaIt.value();

            QStringList parts = key.split(':');
            if (parts.size() == 2) {
                int row = parts[0].toInt();
                int oldCol = parts[1].toInt();

                int newCol = oldCol;
                if (oldCol > currentCol) {
                    newCol = oldCol + 1;
                }

                // Обновляем ссылки в формуле (сдвиг колонок)
                QString updatedFormula = updateColumnReferences(formula, currentCol + 1, 1);

                QString newKey = QString("%1:%2").arg(row).arg(newCol);
                if (del) {
                    del->m_formulas[newKey] = updatedFormula;
                }
            }
        }

    } else {
        // Добавляем пустую колонку в конец
        int newCol = ui->tableWidget->columnCount();
        ui->tableWidget->insertColumn(newCol);
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            ui->tableWidget->setItem(row, newCol, new QTableWidgetItem(""));
        }
    }

    // Обновляем заголовки столбцов
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        ui->tableWidget->setHorizontalHeaderItem(col,
                                                 new QTableWidgetItem(getColumnLetter(col)));
    }

    // Обновляем все формулы
    Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());
    if (del) {
        del->updateAllFormulas(ui->tableWidget->model());
    }
}

void MainWindow::on_actionDeleteColumn_triggered()
{
    int currentCol = ui->tableWidget->currentColumn();

    if (currentCol >= 0) {
        QString colLetter;
        if (currentCol < 26) {
            colLetter = QString(QChar('A' + currentCol));
        } else {
            int first = (currentCol / 26) - 1;
            int second = currentCol % 26;
            colLetter = QString(QChar('A' + first)) + QString(QChar('A' + second));
        }

        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение удаления",
            QString("Вы действительно хотите удалить столбец %1?").arg(colLetter),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            // Сохраняем все данные и формулы
            QMap<QString, QString> allData;
            QMap<QString, QString> allFormulas;

            Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());

            // Сохраняем все данные таблицы (кроме удаляемой колонки)
            for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
                for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
                    if (col == currentCol) continue; // Пропускаем удаляемую колонку

                    QTableWidgetItem* item = ui->tableWidget->item(row, col);
                    if (item) {
                        QString key = QString("%1:%2").arg(row).arg(col);
                        allData[key] = item->text();
                    }
                }
            }

            // Сохраняем все формулы (кроме удаляемой колонки)
            if (del) {
                QMap<QString, QString>::const_iterator formulaIt;
                for (formulaIt = del->m_formulas.begin(); formulaIt != del->m_formulas.end(); ++formulaIt) {
                    QString key = formulaIt.key();
                    QStringList parts = key.split(':');
                    if (parts.size() == 2) {
                        int col = parts[1].toInt();
                        if (col != currentCol) {
                            allFormulas[key] = formulaIt.value();
                        }
                    }
                }
            }

            // Очищаем таблицу
            ui->tableWidget->clearContents();

            // Очищаем формулы
            if (del) {
                del->m_formulas.clear();
            }

            // Удаляем колонку
            ui->tableWidget->removeColumn(currentCol);

            // Восстанавливаем данные со сдвигом влево
            QMap<QString, QString>::const_iterator dataIt;
            for (dataIt = allData.begin(); dataIt != allData.end(); ++dataIt) {
                QString key = dataIt.key();
                QString value = dataIt.value();

                QStringList parts = key.split(':');
                if (parts.size() == 2) {
                    int row = parts[0].toInt();
                    int oldCol = parts[1].toInt();

                    int newCol = oldCol;
                    if (oldCol > currentCol) {
                        newCol = oldCol - 1;
                    }

                    ui->tableWidget->setItem(row, newCol, new QTableWidgetItem(value));
                }
            }

            // Восстанавливаем формулы со сдвигом ссылок
            QMap<QString, QString>::const_iterator formulaIt;
            for (formulaIt = allFormulas.begin(); formulaIt != allFormulas.end(); ++formulaIt) {
                QString key = formulaIt.key();
                QString formula = formulaIt.value();

                QStringList parts = key.split(':');
                if (parts.size() == 2) {
                    int row = parts[0].toInt();
                    int oldCol = parts[1].toInt();

                    int newCol = oldCol;
                    if (oldCol > currentCol) {
                        newCol = oldCol - 1;
                    }

                    // Обновляем ссылки в формуле (сдвиг колонок)
                    QString updatedFormula = updateColumnReferences(formula, currentCol, -1);

                    QString newKey = QString("%1:%2").arg(row).arg(newCol);
                    if (del) {
                        del->m_formulas[newKey] = updatedFormula;
                    }
                }
            }

            // Обновляем заголовки столбцов
            for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
                QString header;
                if (col < 26) {
                    header = QString(QChar('A' + col));
                } else {
                    int first = (col / 26) - 1;
                    int second = col % 26;
                    header = QString(QChar('A' + first)) + QString(QChar('A' + second));
                }
                ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(header));
            }

            // Обновляем все формулы
            if (del) {
                del->updateAllFormulas(ui->tableWidget->model());
            }
        } else {
            ui->statusbar->showMessage("Удаление столбца отменено");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Выберите столбец для удаления");
    }
}

QString MainWindow::updateColumnReferences(const QString &formula, int insertCol, int shift)
{
    QString result = formula;

    // Регулярное выражение для поиска ссылок на ячейки (например, A1, B2, AA10)
    QRegularExpression cellRegex("([A-Z]+)([0-9]+)");
    QRegularExpressionMatchIterator regexIterator = cellRegex.globalMatch(formula);

    QMap<QString, QString> replacements;

    while (regexIterator.hasNext()) {
        QRegularExpressionMatch match = regexIterator.next();
        QString originalRef = match.captured(0);
        QString colStr = match.captured(1);
        QString rowStr = match.captured(2);

        // Конвертируем буквы колонки в число (0-индексация)
        int col = 0;
        for (int i = 0; i < colStr.length(); ++i) {
            col = col * 26 + (colStr[i].toLatin1() - 'A' + 1);
        }
        col--;

        // Обновляем номер колонки если ссылка указывает на колонку
        if (col >= insertCol) {
            int newCol = col + shift;
            if (newCol >= 0) {
                QString newColStr = getColumnLetter(newCol);
                QString newRef = newColStr + rowStr;
                replacements[originalRef] = newRef;
            }
        }
    }

    // Выполняем замены
    QMap<QString, QString>::const_iterator replacementIt;
    for (replacementIt = replacements.begin(); replacementIt != replacements.end(); ++replacementIt) {
        result.replace(replacementIt.key(), replacementIt.value());
    }

    return result;
}

QString MainWindow::getColumnLetter(int col) const
{
    QString result;
    int tempCol = col;
    do {
        result = QString(QChar('A' + (tempCol % 26))) + result;
        tempCol = tempCol / 26 - 1;
    } while (tempCol >= 0);
    return result;
}

void MainWindow::on_actionClearTable_triggered()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение очистки",
        "Вы действительно хотите очистить всю таблицу? Все данные будут удалены.",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        clearTable();
        ui->statusbar->showMessage("Таблица очищена");
    } else {
        ui->statusbar->showMessage("Очистка таблицы отменена");
    }
}

void MainWindow::on_actionFormulaHelp_triggered()
{
    // Создаем диалог с прокруткой
    QDialog *helpDialog = new QDialog(this);
    helpDialog->setWindowTitle("Справка по формулам");
    helpDialog->setMinimumSize(500, 400);
    helpDialog->resize(600, 500);

    // Создаем область прокрутки
    QScrollArea *scrollArea = new QScrollArea(helpDialog);
    scrollArea->setWidgetResizable(true);

    // Создаем виджет с содержимым
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    // Текст справки
    QLabel *helpLabel = new QLabel();
    helpLabel->setTextFormat(Qt::PlainText);
    helpLabel->setWordWrap(true);

    QString helpText =
        "ДОСТУПНЫЕ ФОРМУЛЫ\n"
        "================\n\n"

        "ОСНОВНЫЕ ОПЕРАЦИИ:\n"
        "------------------\n"
        "=A1+B2        - Сложение\n"
        "=A1-B2        - Вычитание\n"
        "=A1*B2        - Умножение\n"
        "=A1/B2        - Деление\n"
        "=A1+B2*C3     - Сложные выражения\n"
        "=10+20        - Прямые вычисления\n\n"

        "ФУНКЦИИ ДИАПАЗОНОВ:\n"
        "------------------\n"
        "=SUM(A1:A10)       - Сумма чисел в диапазоне\n"
        "=AVERAGE(B1:B20)   - Среднее арифметическое\n"
        "=MEDIAN(C1:C15)    - Медиана чисел\n\n"

        "ФУНКЦИИ С ОДНОЙ ЯЧЕЙКОЙ:\n"
        "-----------------------\n"
        "=TOLOWER(A1)       - Перевод текста в нижний регистр\n"
        "=TOUPPER(B2)       - Перевод текста в ВЕРХНИЙ регистр\n\n";

    helpLabel->setText(helpText);

    contentLayout->addWidget(helpLabel);

    scrollArea->setWidget(contentWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(helpDialog);
    mainLayout->addWidget(scrollArea);

    helpDialog->exec();

    delete helpDialog;
}

void MainWindow::clearTable()
{
    ui->tableWidget->clearContents();

    Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());
    if (del) {
        del->m_formulas.clear();
    }
}

void MainWindow::loadCSV(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    clearTable();
    QTextStream stream(&file);
    int row = 0;

    while (!stream.atEnd() && row < ui->tableWidget->rowCount()) {
        QString line = stream.readLine();
        QStringList fields = line.split(',');

        for (int col = 0; col < fields.size() && col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem* item = new QTableWidgetItem(fields[col].trimmed());
            ui->tableWidget->setItem(row, col, item);
        }
        row++;
    }

    file.close();
    ui->statusbar->showMessage("Загружен файл: " + QFileInfo(filename).fileName());
}

void MainWindow::loadJSON(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        QMessageBox::warning(this, "Ошибка", "Неверный формат JSON");
        return;
    }

    clearTable();
    QJsonArray jsonArray = doc.array();

    for (int row = 0; row < jsonArray.size() && row < ui->tableWidget->rowCount(); ++row) {
        QJsonValue value = jsonArray[row];
        if (value.isArray()) {
            QJsonArray rowArray = value.toArray();
            for (int col = 0; col < rowArray.size() && col < ui->tableWidget->columnCount(); ++col) {
                QJsonValue cellValue = rowArray[col];
                QString text;
                if (cellValue.isString()) {
                    text = cellValue.toString();
                } else if (cellValue.isDouble()) {
                    text = QString::number(cellValue.toDouble());
                } else {
                    text = cellValue.toVariant().toString();
                }
                ui->tableWidget->setItem(row, col, new QTableWidgetItem(text));
            }
        }
    }
    ui->statusbar->showMessage("Загружен файл: " + QFileInfo(filename).fileName());
}

void MainWindow::saveCSV(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QTextStream stream(&file);

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            rowData << (item ? item->text() : "");
        }
        stream << rowData.join(',') << "\n";
    }

    file.close();
}

void MainWindow::saveJSON(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QJsonArray jsonArray;

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QJsonArray rowArray;
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            rowArray.append(item ? item->text() : "");
        }
        jsonArray.append(rowArray);
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
    file.close();
}
