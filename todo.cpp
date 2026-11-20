#include <iostream>   // для ввода/вывода (cout, cin)
#include <vector>     // для динамического массива (наш список задач)
#include <string>     // для работы со строками (описание задачи)
#include <fstream>    // для работы с файлами (сохранение/загрузка)
#include <limits>     // для числовых пределов (нужно для очистки буфера ввода)
#include <cstdlib>    // для работы с системными функциями (system())

#ifdef _WIN32
#include <windows.h>  // для переключения консоли в UTF-8
#endif

using namespace std;

struct Task 
{
    string description; // описание задачи
    bool isCompleted; // выполнена или нет
};

const string FILENAME = "tasks.txt"; // файл, в котором хранятся задачи

// Прототипы функций
void clearScreen();
void displayMenu();
int readInt(const string &prompt);
string addTask(vector<Task> &tasks);
void viewTasks(const vector<Task> &tasks);
string markComplete(vector<Task> &tasks);
string deleteTask(vector<Task> &tasks);
void saveTasksToFile(const vector<Task> &tasks, const string &strfilename);
void loadTasksFromFile(vector<Task> &tasks, const string &strfilename);

int main(int argc, char* argv[])
{
#ifdef _WIN32
    // Чтобы русский текст и символы ✓ ✗ нормально отображались в консоли Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    vector<Task> tasks;
    loadTasksFromFile(tasks, FILENAME);

    string message; // результат последнего действия, показывается под списком задач
    bool running = true;
    while (running)
    {
        clearScreen();
        displayMenu();
        viewTasks(tasks); // список задач всегда виден под меню

        if (!message.empty())
        {
            cout << message << endl;
            message.clear();
        }

        int choice = readInt("Ваш выбор: ");

        switch (choice)
        {
            case 1:
                message = addTask(tasks);
                break;
            case 2:
                message = markComplete(tasks);
                break;
            case 3:
                message = deleteTask(tasks);
                break;
            case 4:
                saveTasksToFile(tasks, FILENAME);
                message = "Задачи сохранены";
                break;
            case 5:
                saveTasksToFile(tasks, FILENAME);
                cout << "Задачи сохранены. До встречи!" << endl;
                running = false;
                break;
            default:
                message = "Нет такого пункта меню, попробуйте ещё раз.";
                break;
        }
    }
    
    return 0;
}

// Функция для очистки экрана
void clearScreen()
{
#ifdef _WIN32
    system("cls"); // для Windows
#else
    system("clear"); // для Linux / macOS
#endif
}

void displayMenu()
{
    cout << "To-Do List Manager" << endl;
    cout << "---------------------" << endl;
    cout << "1. Добавить задачу" << endl;
    cout << "2. Завершить задачу" << endl;
    cout << "3. Удалить задачу" << endl;
    cout << "4. Сохранить" << endl;
    cout << "5. Сохранить и выйти" << endl;
    cout << "---------------------" << endl;
}

// Безопасное чтение целого числа: повторяет запрос, пока не введено число
int readInt(const string &prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            // выбрасываем остаток строки (в т.ч. символ '\n'), чтобы не мешал getline
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            return value;
        }

        if (cin.eof())
        {
            return 5; // конец ввода (Ctrl+Z / Ctrl+D) - считаем как "Сохранить и выйти"
        }

        cin.clear(); // сбрасываем флаг ошибки
        cin.ignore((numeric_limits<streamsize>::max)(), '\n'); // выбрасываем мусор
        cout << "Нужно ввести число." << endl;
    }
}

// Все функции-действия возвращают сообщение о результате, а main() покажет его после перерисовки экрана
string addTask(vector<Task> &tasks)
{
    cout << "Введите описание задачи (пустая строка - отмена): ";
    string description;
    getline(cin, description);
    
    if (description.empty())
    {
        return "Добавление отменено";
    }

    tasks.push_back({description, false});
    return "Задача успешно добавлена";
}

void viewTasks(const vector<Task> &tasks)
{
    cout << "\n--- Задачи ---" << endl;
    if (tasks.empty())
    {
        cout << "Задачи не найдены" << endl;
    }
    else
    {
        for (size_t i = 0; i < tasks.size(); ++i)
        {
            cout << "[" << (i + 1) << "] ";
            cout << (tasks[i].isCompleted ? "✓" : "✗");
            cout << " " << tasks[i].description << endl;
        }
    }
    cout << "---------------------" << endl;
}

string markComplete(vector<Task> &tasks)
{
    if (tasks.empty())
    {
        return "Нет задач, которые можно завершить";
    }

    int number = readInt("Номер задачи, которую нужно завершить: ");

    if (number < 1 || number > static_cast<int>(tasks.size())) // static_cast<int>(tasks.size()) приводит к знаковому int для корректного сравнения
    {
        return "Задачи с таким номером нет";
    }

    Task &task = tasks[number - 1];
    if (task.isCompleted)
    {
        return "Эта задача уже выполнена";
    }

    task.isCompleted = true;
    return "Задача отмечена как выполненная";
}

string deleteTask(vector<Task> &tasks)
{
    if (tasks.empty())
    {
        return "Нет задач, которые можно удалить";
    }

    int number = readInt("Номер задачи, которую нужно удалить: ");

    if (number < 1 || number > static_cast<int>(tasks.size())) // static_cast<int>(tasks.size()) приводит к знаковому int для корректного сравнения
    {
        return "Задачи с таким номером нет";
    }

    tasks.erase(tasks.begin() + (number - 1));
    return "Задача удалена";
}

// Формат файла: одна задача на строку -> "<0 или 1> <описание>"
void saveTasksToFile(const vector<Task> &tasks, const string &strfilename)
{
    ofstream file(strfilename);
    if (!file)
    {
        cout << "Не удалось открыть файл для записи: " << strfilename << endl;
        return;
    }

    for (const Task &task : tasks)
    {
        file << (task.isCompleted ? 1 : 0) << ' ' << task.description << '\n';
    }
}

void loadTasksFromFile(vector<Task> &tasks, const string &strfilename)
{
    ifstream file(strfilename);
    if (!file)
    {
        return; // файла ещё нет (первый запуск) - это нормально
    }

    tasks.clear(); // удаляет все текущие задачи

    int status;
    while (file >> status)
    {
        file.ignore(1); // пропускаем пробел между статусом и описанием (пропускает ровно один символ, то есть этот пробел)
        string description;
        getline(file, description);

        if (!description.empty())
        {
            tasks.push_back({description, status == 1});
        }
    }
}