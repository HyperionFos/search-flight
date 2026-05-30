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

int main() {
    std::vector<Flight> arr = generateFlights(200, 52);
    std::string key = arr[0].airline; 
    std::cout << "Looking for: " << key << "\n";

    std::vector<const Flight*> found = linearSearch(arr, key);
    std::cout << "Found: " << found.size() << "\n";
    for (const Flight* f : found) {
        std::cout << "  " << f->flightNumber << " | " << f->airline
                  << " | " << f->arrivalDate << " " << f->arrivalTime
                  << " | passengers = " << f->passengers << "\n";
    }
    return 0;
}