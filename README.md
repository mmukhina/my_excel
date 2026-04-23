### Постановка задачи
Реализовать программу в виде оконного приложения, реализующего работу с таблицами.
Реализовать ввод таблицы - создание столбцов и добавление данных в самом окне или выгрузку из csv или json файлов; редактирование таблицы в окне; применение функций к данным; сохранение итоговой таблицы в файл.
Функции - сумма, среднее, медиана, для строк - приведение к нижнему или верхнему регистру.
Язык программирования - C++.
Разрешается использовать любые доступные фреймворки для построения оконных приложений на C++.
Запрещается использовать готовые библиотечные решения для работы с данными.

<br>

### Компиляция
```
qmake myExcel.pro
make ./myExcel
```
или (если используется Qt Creator)
Открыть файл myExcel.pro в Qt Creator -> Build -> Run

<br>

### Доступные функции

**Основные операции:**

=A1+B2        - Сложение

=A1-B2        - Вычитание

=A1*B2        - Умножение

=A1/B2        - Деление

=A1+B2*C3     - Сложные выражения

=10+20        - Прямые вычисления

**Функции диапазонов:**

=SUM(A1:A10)       - Сумма чисел в диапазоне

=AVERAGE(B1:B20)   - Среднее арифметическое

=MEDIAN(C1:C15)    - Медиана чисел

**Функции с одной ячейкой:**

=TOLOWER(A1)       - Перевод текста в нижний регистр

=TOUPPER(B2)       - Перевод текста в ВЕРХНИЙ регистр

<br>

### Работа программы:
Главное окно <br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/70b55452-bbcc-4c26-8a70-b217c9301fcd" />
<br><br>
Сложение ячеек<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/99e3a2bf-c4f0-4b7f-a709-3670374c2544" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/36f8ff59-4939-4666-bfdb-66cd9a5325da" />
<br><br>
Вычитание ячеек<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/dde2deaf-c384-41d0-b727-8841d131ae21" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/a7543a8c-b4fc-4391-8825-2dcb7ff0445b" />
<br><br>
Сложное выражение<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/5109a7ef-58f1-4d3f-ac25-e1149650c08e" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/09cc6f00-a6d3-4ae6-b957-ab44db33842c" />
<br><br>
Сумма диапазона<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/68665b04-c40f-4204-9ba2-6121a9e08401" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/df0fcde1-e21d-4982-8aa5-5b92a468eeaa" />
<br><br>
Среднее диапазона<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/814024bb-da97-4573-8297-65b73f7088d8" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/4c00ade8-fff0-4133-bb89-004dfe964d81" />
<br><br>
Медиана диапазона<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/278d70c8-9fd2-42a6-a3cd-baa349ae92c6" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/9a552a21-62b8-492c-a94b-ce4500f1809a" />
<br><br>
Перевод текста в вехний регистр<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/cdf05299-2aff-4a65-b00c-1ddb9b1cad8e" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/4470ade3-ee31-421c-b833-761a807843db" />
<br><br>
Перевод текста в нижний регистр<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/4d936b5a-c67d-4ac2-888c-bf929b4b00b1" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/afef976a-2b61-4de4-94ab-9eaa39c135de" />
<br><br>
Сохранение в файл CSV<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/be3fb29f-1393-4405-bee9-b5b2bc9b24c1" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/bf3a28d7-e6ef-4a62-a907-e5ffaf78fccc" />
<br><br>
Сохранение в файл JSON<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/1bd49e1f-42a7-423a-a54b-f4405159ecb1" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/0fab91d6-91ea-42ff-b60f-c46f179084dd" />
<br><br>
Добавить строку<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/1463fbb7-70b0-4612-bdd7-be473843261b" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/784535a9-9572-4a5a-b5bf-de0056a72f0f" />
<br><br>
Удалить строку<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/b621b6f9-a3c0-4a20-a11d-b7bb5eb7c480" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/6f03193c-7395-40d5-94b3-493163dcbfc4" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/d9f0702b-05b5-4a2e-afc3-b58002813a0b" />
<br><br>
Справка по формулам<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/1cbffd6e-cf30-454f-adce-14d9a79c4ea5" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/c14d8157-761d-48a6-ac6e-48ebe01b6729" />
<br><br>
Загрузка CSV файла<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/e4152ab3-a907-4654-8022-283c1cfe7432" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/a45c7954-b733-4eea-80e5-0e3a7b430608" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/73bfe054-7bde-42e9-bcc0-11290fd4079d" />
<br><br>
Неправильно введенная формула<br>
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/75033608-4b80-4616-baa1-44ed4a60a0ac" />
<img width="49%" alt="image" src="https://github.com/user-attachments/assets/cfd4c769-90cc-4e3c-9067-78145fe8ba27" />


