#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Главное окно табличного редактора
 *
 * Предоставляет функциональность для:
 * - Отображения и редактирования таблицы
 * - Загрузки/сохранения в CSV и JSON форматах
 * - Добавления/удаления строк и столбцов
 * - Обновления ссылок в формулах при изменении структуры таблицы
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Действия меню "Файл"
    void on_actionNew_triggered();
    void on_actionOpenCSV_triggered();
    void on_actionOpenJSON_triggered();
    void on_actionSaveAsCSV_triggered();
    void on_actionSaveAsJSON_triggered();
    void on_actionExit_triggered();

    // Действия меню "Правка"
    void on_actionAddRow_triggered();
    void on_actionDeleteRow_triggered();
    void on_actionAddColumn_triggered();
    void on_actionDeleteColumn_triggered();
    void on_actionClearTable_triggered();

    // Действия меню "Справка"
    void on_actionFormulaHelp_triggered();

private:
    Ui::MainWindow *ui;  ///< Указатель на UI главного окна

    /**
     * @brief Загружает таблицу из CSV файла
     * @param filename Путь к файлу
     */
    void loadCSV(const QString& filename);

    /**
     * @brief Загружает таблицу из JSON файла
     * @param filename Путь к файлу
     */
    void loadJSON(const QString& filename);

    /**
     * @brief Сохраняет таблицу в CSV файл
     * @param filename Путь к файлу
     */
    void saveCSV(const QString& filename);

    /**
     * @brief Сохраняет таблицу в JSON файл
     * @param filename Путь к файлу
     */
    void saveJSON(const QString& filename);

    /**
     * @brief Очищает содержимое таблицы
     */
    void clearTable();

    /**
     * @brief Обновляет ссылки на строки в формуле при добавлении/удалении строк
     * @param formula Исходная формула
     * @param insertRow Строка, начиная с которой происходит сдвиг
     * @param shift Величина сдвига (+1 для вставки, -1 для удаления)
     * @return Формула с обновленными ссылками
     */
    QString updateFormulaReferences(const QString &formula, int insertRow, int shift);

    /**
     * @brief Обновляет ссылки на столбцы в формуле при добавлении/удалении столбцов
     * @param formula Исходная формула
     * @param insertCol Столбец, начиная с которого происходит сдвиг
     * @param shift Величина сдвига (+1 для вставки, -1 для удаления)
     * @return Формула с обновленными ссылками
     */
    QString updateColumnReferences(const QString &formula, int insertCol, int shift);

    /**
     * @brief Преобразует номер столбца в буквенное обозначение
     * @param col Номер столбца (0-индексация)
     * @return Буквенное обозначение (A, B, ..., Z, AA, AB, ...)
     */
    QString getColumnLetter(int col) const;
};

#endif
