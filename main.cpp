#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>


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

int main() {
    std::vector<Flight> arr = generateFlights(2000, 52);

    BST tree;
    for (const Flight& f : arr) {
        tree.insert(f);
    }

    std::string key = arr[0].airline;
    std::cout << "Looking for : " << key << "\n";

    std::vector<const Flight*> found = tree.search(key);
    std::cout << "BST found: " << found.size() << "\n";

    // сверим с линейным поиском — должно совпасть
    std::vector<const Flight*> check = linearSearch(arr, key);
    std::cout << "Linear found: " << check.size() << "\n";

    return 0;
}