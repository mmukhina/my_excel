#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "delegate.h"
#include <QTableWidgetItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // IMPORTANT: Apply ONE delegate that does EVERYTHING
    Delegate *delegate = new Delegate(this);
    ui->tableWidget->setItemDelegate(delegate);  // Same delegate for entire table

    // Connect cell changed signal to debug
    connect(ui->tableWidget, &QTableWidget::cellChanged,
            this, &MainWindow::on_tableWidget_cellChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    QTableWidgetItem *item = ui->tableWidget->item(row, column);
    if (item) {
        qDebug() << "Cell" << row << "," << column << "changed to:" << item->text();
    }
}
