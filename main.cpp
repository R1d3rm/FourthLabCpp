#include <iostream>
#include <map>
#include <memory>
#include "header/my_allocator.h"
#include "header/my_container.h"

// Функция для вычисления факториала
long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    // 1) Создание экземпляра std::map<int, int>
    std::map<int, int> standardMap;
    
    // 2) Заполнение 10 элементами, где ключ – это число от 0 до 9, а значение – факториал ключа
    for (int i = 0; i < 10; ++i) {
        standardMap[i] = factorial(i);
    }
    
    // 3) Создание экземпляра std::map<int, int> с новым аллокатором, ограниченным 10 элементами
    using MapAllocator = my_allocator<std::pair<const int, int>, 10>;
    std::map<int, int, std::less<int>, MapAllocator> customMap;
    
    // 4) Заполнение 10 элементами, где ключ – это число от 0 до 9, а значение – факториал ключа
    for (int i = 0; i < 10; ++i) {
        customMap[i] = factorial(i);
    }
    
    // 5) Вывод на экран всех значений(ключ и значение разделены пробелом), хранящихся в контейнере
    std::cout << "Стандартный map:" << std::endl;
    for (const auto& pair : standardMap) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
    
    std::cout << "\nСтандартный map с моим аллокатором:" << std::endl;
    for (const auto& pair : customMap) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
    
    // 6) Создание экземпляра своего контейнера для хранения значений типа int
    MyContainer<int> standardContainer;
    
    // 7) Заполнение 10 элементами от 0 до 9
    for (int i = 0; i < 10; ++i) {
        standardContainer.push_back(i);
    }
    
    // 8) Создание экземпляра своего контейнера для хранения значений типа int с новым аллокатором, ограниченным 10 элементами
    using ContainerAllocator = my_allocator<int, 10>;
    MyContainer<int, ContainerAllocator> customContainer;
    
    // 9) Заполнение 10 элементами от 0 до 9
    for (int i = 0; i < 10; ++i) {
        customContainer.push_back(i);
    }
    
    // 10) Вывод на экран всех значений, хранящихся в контейнере
    std::cout << "\nМой контейнер со стандартным аллокатором:" << std::endl;
    for (auto it = standardContainer.begin(); it != standardContainer.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout<<std::endl;
    std::cout << "\nМой контейнер с моим аллокатором:" << std::endl;
    for (auto it = customContainer.begin(); it != customContainer.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout<<std::endl;
    return 0;
}