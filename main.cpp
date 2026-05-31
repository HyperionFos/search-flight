#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <map>


/**
 * @brief Структура единичное прилета самолета
 * 
 * Все поля хранятся в формате string, дата и время используют
 * фиксированный по ширине формат "ГГГГ-ММ-ДД" и "ЧЧ:ММ" соответственно,
 * чтобы при сравнении не было ошибок т.к. 3:20 < 11:20, но 3 > 1 (сравниваются первые символы)
 * 
 */
struct Flight {
    std::string flightNumber;
    std::string airline; // наш ключ
    std::string arrivalDate;
    std::string arrivalTime;
    int passengers = 0;
};

/**
 * @brief Генерирует массив из n случайных рейсов.
 * 
 * @param n размер массива
 * @param rng генератор случайных чисел
 * @return вектор Flight; airline повторяется (~50 вариантов),
 * flightNumber уникален (префикс компании + порядковый номер).
 */
std::vector<Flight> generateFlights(size_t n, unsigned seed) {
    std::mt19937 rng(seed);
    std::vector<std::string> airlines = {
        "Aeroflot", "S7", "Pobeda", "UralAirlines", "Utair", "RedWings", "Azimuth",
        "Lufthansa", "AirFrance", "KLM", "BritishAirways", "Turkish", "Emirates",
        "Qatar", "Etihad", "Delta", "United", "American", "Southwest", "Ryanair",
        "EasyJet", "Wizzair", "Iberia", "Alitalia", "SAS", "Finnair", "Austrian",
        "Swiss", "Brussels", "TAP", "LOT", "CzechAir", "AirChina", "ChinaEastern",
        "ChinaSouthern", "ANA", "JAL", "KoreanAir", "Asiana", "Singapore",
        "Cathay", "Thai", "Qantas", "AirIndia", "Indigo", "Egyptair", "Ethiopian",
        "NordWind", "AzurAir", "Yamal"
    };
    std::uniform_int_distribution<int> airlineIdx(0, (int)airlines.size() - 1);
    std::uniform_int_distribution<int> day(1, 28);
    std::uniform_int_distribution<int> month(1, 12);
    std::uniform_int_distribution<int> hour(0, 23);
    std::uniform_int_distribution<int> minute(0, 59);
    std::uniform_int_distribution<int> passengers(50, 350);
    std::uniform_int_distribution<int> flightNo(100, 9999);

    std::vector<Flight> flights;
    flights.reserve(n);

    for (size_t i = 0; i < n; ++i) {
        Flight f;
        f.airline = airlines[airlineIdx(rng)];
        f.flightNumber = f.airline.substr(0, 2) + std::to_string(i);

        std::ostringstream date;
        date << "2026-"
             << std::setw(2) << std::setfill('0') << month(rng) << "-"
             << std::setw(2) << std::setfill('0') << day(rng);
        f.arrivalDate = date.str();

        std::ostringstream time;
        time << std::setw(2) << std::setfill('0') << hour(rng) << ":"
             << std::setw(2) << std::setfill('0') << minute(rng);
        f.arrivalTime = time.str();

        f.passengers = passengers(rng);
        flights.push_back(f);
    }
    return flights;
}

/**
 * @brief Линейный поиск всех рейсов по ключу (airline)
 *
 * Проходит по всему массиву и собирает указатели на все элементы,
 * у которых поле airline совпадает с искомым ключом. Так как ключи
 * не уникальны, результатом может быть несколько элементов.
 *
 * Сложность: O(n) на один запрос.
 *
 * @param flights массив рейсов, в котором ищем
 * @param key значение ключа (название авиакомпании)
 * @return вектор указателей на найденные рейсы (пустой, если ничего не найдено)
 */
std::vector<const Flight*> linearSearch(const std::vector<Flight>& flights,
                                        const std::string& key) {
    std::vector<const Flight*> result;
    for (const Flight& f : flights) {
        if (f.airline == key) {
            result.push_back(&f);
        }
    }
    return result;
}

/**
 * @brief Бинарное дерево поиска (BST) по ключу airline
 *
 * Несбалансированное дерево. Так как ключи (airline) не уникальны,
 * каждый узел хранит вектор ВСЕХ рейсов с данным ключом. Поэтому поиск
 * всех вхождений сводится к поиску одного узла.
 *
 * Сложность поиска: O(h), где h — высота дерева (в среднем O(log n)).
 */
class BST {
private:
    struct Node {
        std::string key;               // ключ — название авиакомпании
        std::vector<Flight> items;     // все рейсы с этим ключом
        Node* left = nullptr;
        Node* right = nullptr;
        Node(const std::string& k) : key(k) {}
    };

    Node* root = nullptr;

    /**
     * @brief Рекурсивно освобождает память поддерева
     * @param node корень поддерева
     */
    void destroy(Node* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    BST() = default;
    ~BST() { destroy(root); }

    /**
     * @brief Вставка рейса в дерево
     *
     * Спускается по дереву, сравнивая ключи. Если узел с таким ключом
     * найден — добавляет рейс в его вектор. Иначе создает новый узел.
     *
     * @param f рейс для вставки
     */
    void insert(const Flight& f) {
        if (root == nullptr) {
            root = new Node(f.airline);
            root->items.push_back(f);
            return;
        }
        Node* cur = root;
        while (true) {
            if (f.airline < cur->key) {
                if (cur->left == nullptr) {
                    cur->left = new Node(f.airline);
                    cur->left->items.push_back(f);
                    return;
                }
                cur = cur->left;
            } else if (f.airline > cur->key) {
                if (cur->right == nullptr) {
                    cur->right = new Node(f.airline);
                    cur->right->items.push_back(f);
                    return;
                }
                cur = cur->right;
            } else {
                cur->items.push_back(f); // ключ уже есть — добавляем в вектор
                return;
            }
        }
    }

    /**
     * @brief Поиск всех рейсов по ключу
     *
     * Спускается по дереву, пока не найдет узел с нужным ключом.
     *
     * @param key искомое название авиакомпании
     * @return вектор указателей на найденные рейсы (пустой, если не найдено)
     */
    std::vector<const Flight*> search(const std::string& key) const {
        std::vector<const Flight*> result;
        Node* cur = root;
        while (cur != nullptr) {
            if (key < cur->key) {
                cur = cur->left;
            } else if (key > cur->key) {
                cur = cur->right;
            } else {
                for (const Flight& f : cur->items) {
                    result.push_back(&f);
                }
                return result;
            }
        }
        return result;
    }
};

/**
 * @brief Красно-чёрное дерево (RBT) по ключу airline
 *
 * Самобалансирующееся дерево поиска. После каждой вставки восстанавливает
 * 5 свойств RBT, что гарантирует высоту O(log n) и поиск за O(log n) даже
 * в худшем случае. Ключи (airline) не уникальны — каждый узел хранит
 * вектор всех рейсов с данным ключом.
 *
 * Используется узел-страж nil (общий чёрный лист) — это убирает проверки
 * на nullptr: концы дерева и "отсутствующий дядя" указывают на nil.
 */
class RBTree {
private:
    enum Color { RED, BLACK };

    struct Node {
        std::string key;
        std::vector<Flight> items;
        Color color = RED;          // новый узел всегда красный
        Node* left;
        Node* right;
        Node* parent;
        Node(const std::string& k) : key(k) {}
    };

    Node* root;
    Node* nil;   // страж: общий чёрный лист

    /**
     * @brief Левый поворот вокруг узла x (правый ребёнок поднимается)
     */
    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil)            root = y;
        else if (x == x->parent->left)   x->parent->left = y;
        else                             x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    /**
     * @brief Правый поворот вокруг узла x (левый ребёнок поднимается)
     */
    void rotateRight(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil) y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nil)            root = y;
        else if (x == x->parent->right)  x->parent->right = y;
        else                             x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    /**
     * @brief Восстановление свойств RBT после вставки узла z
     *
     * Цикл работает, пока родитель z красный (нарушено правило 4).
     * Внутри — разбор по цвету дяди: красный дядя (случай 1) — перекраска
     * и подъём вверх; чёрный дядя (случаи 2 и 3) — повороты и перекраска.
     */
    void insertFixup(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* uncle = z->parent->parent->right;
                if (uncle->color == RED) {                 // случай 1
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {           // случай 2 (зигзаг)
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->color = BLACK;              // случай 3 (прямая)
                    z->parent->parent->color = RED;
                    rotateRight(z->parent->parent);
                }
            } else { // зеркально: родитель — правый ребёнок деда
                Node* uncle = z->parent->parent->left;
                if (uncle->color == RED) {                 // случай 1
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {            // случай 2 (зигзаг)
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->color = BLACK;              // случай 3 (прямая)
                    z->parent->parent->color = RED;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->color = BLACK;   // правило 2: корень всегда чёрный
    }

    /**
     * @brief Рекурсивно освобождает память поддерева
     */
    void destroy(Node* node) {
        if (node == nil) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    RBTree() {
        nil = new Node("");
        nil->color = BLACK;
        nil->left = nil->right = nil->parent = nil;
        root = nil;
    }
    ~RBTree() {
        destroy(root);
        delete nil;
    }

    /**
     * @brief Вставка рейса
     *
     * Сначала обычный спуск BST до места вставки. Если ключ уже есть —
     * добавляем рейс в вектор узла. Иначе создаём новый красный узел
     * и запускаем восстановление свойств.
     *
     * @param f рейс для вставки
     */
    void insert(const Flight& f) {
        Node* y = nil;
        Node* x = root;
        while (x != nil) {
            y = x;
            if (f.airline < x->key)       x = x->left;
            else if (f.airline > x->key)  x = x->right;
            else { x->items.push_back(f); return; }  // ключ уже есть
        }
        Node* z = new Node(f.airline);
        z->items.push_back(f);
        z->parent = y;
        z->left = nil;
        z->right = nil;
        if (y == nil)                 root = z;       // дерево было пустым
        else if (f.airline < y->key)  y->left = z;
        else                          y->right = z;
        insertFixup(z);
    }

    /**
     * @brief Поиск всех рейсов по ключу
     * @param key искомое название авиакомпании
     * @return вектор указателей на найденные рейсы
     */
    std::vector<const Flight*> search(const std::string& key) const {
        std::vector<const Flight*> result;
        Node* cur = root;
        while (cur != nil) {
            if (key < cur->key)       cur = cur->left;
            else if (key > cur->key)  cur = cur->right;
            else {
                for (const Flight& f : cur->items) result.push_back(&f);
                return result;
            }
        }
        return result;
    }
};

/**
 * @brief Хэш-таблица по ключу airline с разрешением коллизий цепочками
 *
 * Каждая корзина (bucket) — связанный список узлов. Хэш-функция —
 * полиномиальная свёртка строки (схема Горнера, множитель 31) по модулю
 * размера таблицы. Ключи не уникальны: узел хранит вектор всех рейсов
 * с данным ключом. Коллизии (новый ключ попал в непустую корзину)
 * подсчитываются.
 *
 * Сложность поиска: O(1) в среднем.
 */
class HashTable {
private:
    struct Node {
        std::string key;
        std::vector<Flight> items;
        Node* next = nullptr;
        Node(const std::string& k) : key(k) {}
    };

    std::vector<Node*> buckets;
    size_t tableSize;
    long long collisions = 0;

    /**
     * @brief Хэш-функция: строка -> индекс корзины
     *
     * Полиномиальный хэш (схема Горнера): h = h * 31 + символ.
     * В конце берётся остаток по размеру таблицы.
     *
     * @param key ключ (название авиакомпании)
     * @return индекс корзины в диапазоне [0, tableSize)
     */
    size_t hash(const std::string& key) const {
        size_t h = 0;
        for (unsigned char c : key) {
            h = h * 31 + c;
        }
        return h % tableSize;
    }

public:
    /**
     * @brief Конструктор
     * @param size количество корзин в таблице
     */
    HashTable(size_t size) : buckets(size, nullptr), tableSize(size) {}

    ~HashTable() {
        for (Node* head : buckets) {
            while (head != nullptr) {
                Node* next = head->next;
                delete head;
                head = next;
            }
        }
    }

    /**
     * @brief Вставка рейса
     *
     * Считает индекс корзины. Если ключ уже есть в корзине — добавляет рейс
     * в его вектор. Иначе создаёт новый узел; если корзина была непуста —
     * фиксирует коллизию.
     *
     * @param f рейс для вставки
     */
    void insert(const Flight& f) {
        size_t idx = hash(f.airline);
        // ищем узел с таким же ключом в цепочке
        for (Node* n = buckets[idx]; n != nullptr; n = n->next) {
            if (n->key == f.airline) {
                n->items.push_back(f);
                return;
            }
        }
        // новый ключ: если корзина уже занята — это коллизия
        if (buckets[idx] != nullptr) {
            collisions++;
        }
        Node* node = new Node(f.airline);
        node->items.push_back(f);
        node->next = buckets[idx];   // вставка в начало цепочки
        buckets[idx] = node;
    }

    /**
     * @brief Поиск всех рейсов по ключу
     * @param key искомое название авиакомпании
     * @return вектор указателей на найденные рейсы
     */
    std::vector<const Flight*> search(const std::string& key) const {
        std::vector<const Flight*> result;
        size_t idx = hash(key);
        for (Node* n = buckets[idx]; n != nullptr; n = n->next) {
            if (n->key == key) {
                for (const Flight& f : n->items) {
                    result.push_back(&f);
                }
                return result;
            }
        }
        return result;
    }

    /**
     * @brief Число зафиксированных коллизий
     */
    long long getCollisions() const { return collisions; }
};

/**
 * @brief Поиск всех рейсов по ключу в multimap
 *
 * Использует equal_range, который возвращает диапазон
 * всех записей с заданным ключом, и собирает их.
 *
 * @param mm ассоциативный массив "airline -> Flight"
 * @param key искомая авиакомпания
 * @return вектор указателей на найденные рейсы
 */
std::vector<const Flight*> multimapSearch(const std::multimap<std::string, Flight>& mm,
                                          const std::string& key) {
    std::vector<const Flight*> result;
    auto range = mm.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(&it->second);
    }
    return result;
}

int main() {
    std::vector<Flight> arr = generateFlights(20000, 52);

    BST bst;
    RBTree rbt;
    HashTable ht(101);
    std::multimap<std::string, Flight> mm;
    for (const Flight& f : arr) {
        bst.insert(f);
        rbt.insert(f);
        ht.insert(f);
        mm.insert({f.airline, f});
    }

    std::string key = arr[0].airline;
    std::cout << "Looking for: " << key << "\n";
    std::cout << "Linear: " << linearSearch(arr, key).size() << "\n";
    std::cout << "BST: " << bst.search(key).size() << "\n";
    std::cout << "RBT: " << rbt.search(key).size() << "\n";
    std::cout << "Hash: " << ht.search(key).size() << "\n";
    std::cout << "Collisions: " << ht.getCollisions() << "\n";
    std::cout << "Multimap: " << multimapSearch(mm, key).size() << "\n";
    return 0;
}