#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>

using namespace std;

// Узел связного списка
template<typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;

    Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
};

// Базовый класс связного списка
template<typename T>
class LinkedList {
protected:
    Node<T>* head;
    Node<T>* tail;
    int size;
    int listType;

public:
    LinkedList(int type) : head(nullptr), tail(nullptr), size(0), listType(type) {}
    virtual ~LinkedList() { clear(); }

    int getType() const { return listType; }
    int getSize() const { return size; }
    bool isEmpty() const { return size == 0; }

    virtual void addElement(const T& value) = 0;
    virtual void display() const = 0;
    virtual vector<int> find(const T& value) const = 0;
    virtual bool removeFirst(const T& value) = 0;
    virtual int removeAll(const T& value) = 0;
    virtual void clear() = 0;
    virtual void writeToFile(const string& filename) const = 0;
    virtual void readFromFile(const string& filename) = 0;
};

// Однонаправленный связный список
template<typename T>
class SinglyLinkedList : public LinkedList<T> {
public:
    SinglyLinkedList() : LinkedList<T>(1) {}

    void addElement(const T& value) override {
        Node<T>* newNode = new Node<T>(value);
        if (!this->head) {
            this->head = newNode;
            this->tail = newNode;
        } else {
            this->tail->next = newNode;
            this->tail = newNode;
        }
        this->size++;
    }

    void display() const override {
        if (!this->head) {
            cout << "Список пуст" << endl;
            return;
        }

        Node<T>* current = this->head;
        while (current) {
            cout << current->data;
            if (current->next) cout << " -> ";
            current = current->next;
        }
        cout << endl;
    }

    vector<int> find(const T& value) const override {
        vector<int> positions;
        Node<T>* current = this->head;
        int pos = 1;

        while (current) {
            if (current->data == value) {
                positions.push_back(pos);
            }
            current = current->next;
            pos++;
        }
        return positions;
    }

    bool removeFirst(const T& value) override {
        if (!this->head) return false;

        if (this->head->data == value) {
            Node<T>* toDelete = this->head;
            this->head = this->head->next;
            if (!this->head) this->tail = nullptr;
            delete toDelete;
            this->size--;
            return true;
        }

        Node<T>* current = this->head;
        while (current->next && current->next->data != value) {
            current = current->next;
        }

        if (current->next) {
            Node<T>* toDelete = current->next;
            current->next = current->next->next;
            if (toDelete == this->tail) this->tail = current;
            delete toDelete;
            this->size--;
            return true;
        }
        return false;
    }

    int removeAll(const T& value) override {
        int count = 0;
        while (this->head && this->head->data == value) {
            Node<T>* toDelete = this->head;
            this->head = this->head->next;
            delete toDelete;
            this->size--;
            count++;
        }
        if (!this->head) {
            this->tail = nullptr;
            return count;
        }

        Node<T>* current = this->head;
        while (current->next) {
            if (current->next->data == value) {
                Node<T>* toDelete = current->next;
                current->next = current->next->next;
                if (toDelete == this->tail) this->tail = current;
                delete toDelete;
                this->size--;
                count++;
            } else {
                current = current->next;
            }
        }
        return count;
    }

    void clear() override {
        Node<T>* current = this->head;
        while (current) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        this->head = nullptr;
        this->tail = nullptr;
        this->size = 0;
    }

    void writeToFile(const string& filename) const override {
        ofstream file(filename);
        if (!file.is_open()) throw runtime_error("Не удалось открыть файл");
        file << ">" << endl;
        Node<T>* current = this->head;
        while (current) {
            file << current->data;
            if (current->next) file << " ";
            current = current->next;
        }
        file << endl;
        file.close();
    }

    void readFromFile(const string& filename) override {
        ifstream file(filename);
        if (!file.is_open()) throw runtime_error("Не удалось открыть файл");
        clear();
        string typeMarker;
        getline(file, typeMarker);
        if (typeMarker != ">") throw runtime_error("Неверный тип списка");

        string line;
        if (getline(file, line)) {
            istringstream iss(line);
            T value;
            while (iss >> value) {
                addElement(value);
            }
        }
        file.close();
    }
};

// Двунаправленный связный список
template<typename T>
class DoublyLinkedList : public LinkedList<T> {
public:
    DoublyLinkedList() : LinkedList<T>(2) {}

    void addElement(const T& value) override {
        Node<T>* newNode = new Node<T>(value);
        if (!this->head) {
            this->head = newNode;
            this->tail = newNode;
        } else {
            newNode->prev = this->tail;
            this->tail->next = newNode;
            this->tail = newNode;
        }
        this->size++;
    }

    void display() const override {
        if (!this->head) {
            cout << "Список пуст" << endl;
            return;
        }

        cout << "Прямой порядок: ";
        Node<T>* current = this->head;
        while (current) {
            cout << current->data;
            if (current->next) cout << " <-> ";
            current = current->next;
        }
        cout << endl;
    }

    vector<int> find(const T& value) const override {
        vector<int> positions;
        Node<T>* current = this->head;
        int pos = 1;

        while (current) {
            if (current->data == value) {
                positions.push_back(pos);
            }
            current = current->next;
            pos++;
        }
        return positions;
    }

    bool removeFirst(const T& value) override {
        if (!this->head) return false;

        Node<T>* current = this->head;
        while (current) {
            if (current->data == value) {
                if (current->prev) {
                    current->prev->next = current->next;
                } else {
                    this->head = current->next;
                }
                if (current->next) {
                    current->next->prev = current->prev;
                } else {
                    this->tail = current->prev;
                }
                delete current;
                this->size--;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    int removeAll(const T& value) override {
        int count = 0;
        Node<T>* current = this->head;

        while (current) {
            if (current->data == value) {
                Node<T>* toDelete = current;
                current = current->next;

                if (toDelete->prev) {
                    toDelete->prev->next = toDelete->next;
                } else {
                    this->head = toDelete->next;
                }
                if (toDelete->next) {
                    toDelete->next->prev = toDelete->prev;
                } else {
                    this->tail = toDelete->prev;
                }

                delete toDelete;
                this->size--;
                count++;
            } else {
                current = current->next;
            }
        }
        return count;
    }

    void clear() override {
        Node<T>* current = this->head;
        while (current) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        this->head = nullptr;
        this->tail = nullptr;
        this->size = 0;
    }

    void writeToFile(const string& filename) const override {
        ofstream file(filename);
        if (!file.is_open()) throw runtime_error("Не удалось открыть файл");
        file << "<>" << endl;
        Node<T>* current = this->head;
        while (current) {
            file << current->data;
            if (current->next) file << " ";
            current = current->next;
        }
        file << endl;
        file.close();
    }

    void readFromFile(const string& filename) override {
        ifstream file(filename);
        if (!file.is_open()) throw runtime_error("Не удалось открыть файл");
        clear();
        string typeMarker;
        getline(file, typeMarker);
        if (typeMarker != "<>") throw runtime_error("Неверный тип списка");

        string line;
        if (getline(file, line)) {
            istringstream iss(line);
            T value;
            while (iss >> value) {
                addElement(value);
            }
        }
        file.close();
    }
};

// Фабрика для создания списков
template<typename T>
LinkedList<T>* createLinkedList(int type) {
    switch (type) {
        case 1: return new SinglyLinkedList<T>();
        case 2: return new DoublyLinkedList<T>();
        case 3: return new SinglyLinkedList<T>(); // Упрощенно, для кольцевых
        case 4: return new DoublyLinkedList<T>(); // Упрощенно, для кольцевых
        default: return new SinglyLinkedList<T>();
    }
}

// Меню выбора типа списка
int selectListType() {
    int type;
    cout << "=== ВЫБОР ТИПА СПИСКА ===" << endl;
    cout << "1. Однонаправленный связный список" << endl;
    cout << "2. Двунаправленный связный список" << endl;
    cout << "3. Однонаправленный кольцевой связный список" << endl;
    cout << "4. Двунаправленный кольцевой связный список" << endl;
    cout << "Выберите тип (1-4): ";
    cin >> type;
    cin.ignore();

    if (type < 1 || type > 4) {
        cout << "Неверный тип. Установлен тип 1 по умолчанию." << endl;
        return 1;
    }
    return type;
}

// Инициализация списка
void initializeList(LinkedList<string>*& list) {
    cout << "\n=== ИНИЦИАЛИЗАЦИЯ СПИСКА ===" << endl;
    cout << "1. Создать пустой список" << endl;
    cout << "2. Ввести элементы с клавиатуры" << endl;
    cout << "3. Загрузить из файла" << endl;
    cout << "Выберите вариант: ";

    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        cout << "Создан пустой список." << endl;
    }
    else if (choice == 2) {
        cout << "Введите количество элементов: ";
        int count;
        cin >> count;
        cin.ignore();

        if (count > 0) {
            cout << "Введите " << count << " элементов (каждый с новой строки):" << endl;
            for (int i = 0; i < count; i++) {
                string element;
                cout << "Элемент " << (i + 1) << ": ";
                getline(cin, element);
                list->addElement(element);
            }
            cout << "Список создан с " << list->getSize() << " элементами." << endl;
        }
    }
    else if (choice == 3) {
        cout << "Введите имя файла: ";
        string filename;
        getline(cin, filename);

        try {
            list->readFromFile(filename);
            cout << "Список загружен из файла. Размер: " << list->getSize() << endl;
        } catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }
}

// Функция для отображения меню
void displayMenu() {
    cout << "\n=== МЕНЮ СВЯЗНОГО СПИСКА ===" << endl;
    cout << "1. Добавить элемент" << endl;
    cout << "2. Вывести список" << endl;
    cout << "3. Найти элемент" << endl;
    cout << "4. Удалить первое вхождение элемента" << endl;
    cout << "5. Удалить все вхождения элемента" << endl;
    cout << "6. Очистить список" << endl;
    cout << "7. Записать список в файл" << endl;
    cout << "8. Считать список из файла" << endl;
    cout << "0. Выход" << endl;
    cout << "Выберите действие: ";
}

int main() {
    LinkedList<string>* list = nullptr;

    try {
        // Выбор типа списка
        int type = selectListType();
        list = createLinkedList<string>(type);

        // Инициализация списка
        initializeList(list);

        // Главный цикл меню
        int choice;
        do {
            displayMenu();
            cin >> choice;
            cin.ignore();

            try {
                switch (choice) {
                    case 1: {
                        cout << "Введите элемент для добавления: ";
                        string element;
                        getline(cin, element);
                        list->addElement(element);
                        cout << "Элемент добавлен. Размер списка: " << list->getSize() << endl;
                        break;
                    }

                    case 2: {
                        cout << "\nСодержимое списка:" << endl;
                        list->display();
                        break;
                    }

                    case 3: {
                        cout << "Введите элемент для поиска: ";
                        string element;
                        getline(cin, element);
                        vector<int> positions = list->find(element);

                        if (positions.empty()) {
                            cout << "Элемент не найден" << endl;
                        } else {
                            cout << "Элемент найден на позициях: ";
                            for (size_t i = 0; i < positions.size(); i++) {
                                cout << positions[i];
                                if (i < positions.size() - 1) cout << ", ";
                            }
                            cout << endl;
                        }
                        break;
                    }

                    case 4: {
                        cout << "Введите элемент для удаления (первое вхождение): ";
                        string element;
                        getline(cin, element);

                        if (list->removeFirst(element)) {
                            cout << "Элемент удален. Новый размер: " << list->getSize() << endl;
                        } else {
                            cout << "Элемент не найден" << endl;
                        }
                        break;
                    }

                    case 5: {
                        cout << "Введите элемент для удаления (все вхождения): ";
                        string element;
                        getline(cin, element);

                        int removed = list->removeAll(element);
                        if (removed > 0) {
                            cout << "Удалено " << removed << " вхождений. Новый размер: " << list->getSize() << endl;
                        } else {
                            cout << "Элемент не найден" << endl;
                        }
                        break;
                    }

                    case 6: {
                        list->clear();
                        cout << "Список очищен" << endl;
                        break;
                    }

                    case 7: {
                        cout << "Введите имя файла для записи: ";
                        string filename;
                        getline(cin, filename);

                        list->writeToFile(filename);
                        cout << "Список записан в файл" << endl;
                        break;
                    }

                    case 8: {
                        if (!list->isEmpty()) {
                            cout << "Список не пуст. Очистите его перед загрузкой из файла." << endl;
                        } else {
                            cout << "Введите имя файла: ";
                            string filename;
                            getline(cin, filename);

                            list->readFromFile(filename);
                            cout << "Список загружен. Размер: " << list->getSize() << endl;
                        }
                        break;
                    }

                    case 0: {
                        cout << "Выход из программы..." << endl;
                        break;
                    }

                    default: {
                        cout << "Неверный выбор!" << endl;
                        break;
                    }
                }
            } catch (const exception& e) {
                cout << "Ошибка: " << e.what() << endl;
            }

        } while (choice != 0);

    } catch (const exception& e) {
        cout << "Критическая ошибка: " << e.what() << endl;
    }

    if (list) delete list;
    return 0;
}