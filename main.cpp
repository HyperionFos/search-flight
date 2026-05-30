#include <string>
#include <vector>
#include <iostream>
#include <fstream>

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

int main() {
    std::cout << "Search Lab\n";
    return 0;
}