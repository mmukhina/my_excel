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
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Delegate *delegate = new Delegate(this);
    ui->tableWidget->setItemDelegate(delegate);

    // Подключаем сигнал изменения ячейки для обновления формул
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, [this](int row, int column) {
        Q_UNUSED(row);
        Q_UNUSED(column);
        Delegate *del = qobject_cast<Delegate*>(ui->tableWidget->itemDelegate());
        if (del) {
            del->updateAllFormulas(ui->tableWidget->model());
        }
    });

    ui->statusbar->showMessage("Готово. Введите формулу: =SUM(A1:A10) или =A1+B2");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    qDebug() << "New triggered";
    clearTable();
    ui->statusbar->showMessage("Создана новая таблица");
}

void MainWindow::on_actionOpenCSV_triggered()
{
    qDebug() << "Open CSV triggered";
    QString filename = QFileDialog::getOpenFileName(this, "Открыть CSV файл", "", "CSV files (*.csv)");
    qDebug() << "Selected file:" << filename;
    if (!filename.isEmpty()) {
        loadCSV(filename);
    }
}

void MainWindow::on_actionOpenJSON_triggered()
{
    qDebug() << "Open JSON triggered";
    QString filename = QFileDialog::getOpenFileName(this, "Открыть JSON файл", "", "JSON files (*.json)");
    qDebug() << "Selected file:" << filename;
    if (!filename.isEmpty()) {
        loadJSON(filename);
    }
}

void MainWindow::on_actionSaveAsCSV_triggered()
{
    qDebug() << "Save CSV triggered";
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить как CSV", "", "CSV files (*.csv)");
    qDebug() << "Save to file:" << filename;
    if (!filename.isEmpty()) {
        saveCSV(filename);
    }
}

void MainWindow::on_actionSaveAsJSON_triggered()
{
    qDebug() << "Save JSON triggered";
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить как JSON", "", "JSON files (*.json)");
    qDebug() << "Save to file:" << filename;
    if (!filename.isEmpty()) {
        saveJSON(filename);
    }
}

void MainWindow::on_actionExit_triggered()
{
    qDebug() << "Exit triggered";
    close();
}

void MainWindow::on_actionAddRow_triggered()
{
    int currentRow = ui->tableWidget->currentRow();

    if (currentRow >= 0) {
        ui->tableWidget->insertRow(currentRow);
        ui->statusbar->showMessage("Добавлена строка " + QString::number(currentRow + 1));
    } else {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->statusbar->showMessage("Добавлена строка " + QString::number(row + 1));
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
            ui->tableWidget->removeRow(currentRow);
            ui->statusbar->showMessage("Удалена строка " + QString::number(currentRow + 1));
        } else {
            ui->statusbar->showMessage("Удаление строки отменено");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для удаления");
    }
}

void MainWindow::on_actionAddColumn_triggered()
{
    int currentCol = ui->tableWidget->currentColumn();

    if (currentCol >= 0) {
        ui->tableWidget->insertColumn(currentCol + 1);

        for (int col = currentCol + 1; col < ui->tableWidget->columnCount(); ++col) {
            if (col < 26) {
                ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(QString(QChar('A' + col))));
            } else {
                QString header = QString(QChar('A' + (col / 26) - 1)) + QString(QChar('A' + (col % 26)));
                ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(header));
            }
        }

        ui->statusbar->showMessage("Добавлен столбец " + QString(QChar('A' + currentCol + 1)));
    } else {
        int col = ui->tableWidget->columnCount();
        ui->tableWidget->insertColumn(col);

        if (col < 26) {
            ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(QString(QChar('A' + col))));
        } else {
            QString header = QString(QChar('A' + (col / 26) - 1)) + QString(QChar('A' + (col % 26)));
            ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(header));
        }

        ui->statusbar->showMessage("Добавлен столбец " + QString(QChar('A' + col)));
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
            colLetter = QString(QChar('A' + (currentCol / 26) - 1)) + QString(QChar('A' + (currentCol % 26)));
        }

        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение удаления",
            QString("Вы действительно хотите удалить столбец %1?").arg(colLetter),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            ui->tableWidget->removeColumn(currentCol);

            for (int col = currentCol; col < ui->tableWidget->columnCount(); ++col) {
                if (col < 26) {
                    ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(QString(QChar('A' + col))));
                } else {
                    QString header = QString(QChar('A' + (col / 26) - 1)) + QString(QChar('A' + (col % 26)));
                    ui->tableWidget->setHorizontalHeaderItem(col, new QTableWidgetItem(header));
                }
            }

            ui->statusbar->showMessage("Удален столбец " + colLetter);
        } else {
            ui->statusbar->showMessage("Удаление столбца отменено");
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Выберите столбец для удаления");
    }
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

void MainWindow::clearTable()
{
    ui->tableWidget->clearContents();
}

void MainWindow::loadCSV(const QString& filename)
{
    qDebug() << "Loading CSV:" << filename;
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
    qDebug() << "CSV loaded successfully";
}

void MainWindow::loadJSON(const QString& filename)
{
    qDebug() << "Loading JSON:" << filename;
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
    qDebug() << "JSON loaded successfully";
}

void MainWindow::saveCSV(const QString& filename)
{
    qDebug() << "Saving CSV:" << filename;
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
    ui->statusbar->showMessage("Сохранен файл: " + QFileInfo(filename).fileName());
    qDebug() << "CSV saved successfully";
}

void MainWindow::saveJSON(const QString& filename)
{
    qDebug() << "Saving JSON:" << filename;
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
    ui->statusbar->showMessage("Сохранен файл: " + QFileInfo(filename).fileName());
    qDebug() << "JSON saved successfully";
}
