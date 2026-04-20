#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionOpenCSV_triggered();
    void on_actionOpenJSON_triggered();
    void on_actionSaveAsCSV_triggered();
    void on_actionSaveAsJSON_triggered();
    void on_actionExit_triggered();
    void on_actionAddRow_triggered();
    void on_actionDeleteRow_triggered();
    void on_actionAddColumn_triggered();
    void on_actionDeleteColumn_triggered();
    void on_actionClearTable_triggered();

private:
    Ui::MainWindow *ui;

    void loadCSV(const QString& filename);
    void loadJSON(const QString& filename);
    void saveCSV(const QString& filename);
    void saveJSON(const QString& filename);
    void clearTable();
};

#endif
