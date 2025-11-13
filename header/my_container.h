#pragma once

#include <iterator>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <algorithm>
#include <utility>

template <typename T, typename Allocator = std::allocator<T>>
class MyContainer {
private:
    T* data_;
    size_t capacity_;
    size_t size_;
    Allocator alloc;

public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    // Итератор
    class iterator {
    private:
        T* ptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(T* p = nullptr) : ptr(p) {}
        
        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }
        
        iterator& operator++() { ++ptr; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        
        bool operator==(const iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    // Константный итератор
    class const_iterator {
    private:
        const T* ptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const T* p = nullptr) : ptr(p) {}
        const_iterator(const iterator& it) : ptr(it.ptr) {}
        
        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }
        
        const_iterator& operator++() { ++ptr; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
        
        bool operator==(const const_iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
    };

    // Конструктор по умолчанию
    MyContainer() : data_(nullptr), capacity_(0), size_(0) {
        allocate_memory(10); // Начальная емкость
    }

    // Конструктор с аллокатором
    explicit MyContainer(const Allocator& allocator) : data_(nullptr), capacity_(0), size_(0), alloc(allocator) {
        allocate_memory(10); // Начальная емкость
    }

    // Деструктор
    ~MyContainer() {
        clear();
        if (data_) {
            alloc.deallocate(data_, capacity_);
        }
    }

    // Добавление элемента в конец
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            grow();
        }
        alloc.construct(data_ + size_, value);
        ++size_;
    }

    // Добавление элемента в конец (rvalue версия)
    void push_back(T&& value) {
        if (size_ >= capacity_) {
            grow();
        }
        alloc.construct(data_ + size_, std::move(value));
        ++size_;
    }

    // Очистка контейнера
    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            alloc.destroy(data_ + i);
        }
        size_ = 0;
    }

    // Размер контейнера
    size_type size() const noexcept {
        return size_;
    }

    // Проверка на пустоту
    bool empty() const noexcept {
        return size_ == 0;
    }

    // Емкость контейнера
    size_type capacity() const noexcept {
        return capacity_;
    }

    // Получение аллокатора
    allocator_type get_allocator() const {
        return alloc;
    }

    // Итераторы
    iterator begin() noexcept {
        return iterator(data_);
    }

    iterator end() noexcept {
        return iterator(data_ + size_);
    }

    // Константные итераторы
    const_iterator begin() const noexcept {
        return const_iterator(data_);
    }

    const_iterator end() const noexcept {
        return const_iterator(data_ + size_);
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(data_);
    }

    const_iterator cend() const noexcept {
        return const_iterator(data_ + size_);
    }

private:
    // Выделение памяти
    void allocate_memory(size_type new_capacity) {
        if (new_capacity > 0) {
            data_ = alloc.allocate(new_capacity);
            capacity_ = new_capacity;
        }
    }

    // Увеличение размера контейнера
    void grow() {
        size_t new_capacity = capacity_ == 0 ? 10 : capacity_ * 2;
        T* new_data = alloc.allocate(new_capacity);
        
        // Перемещение существующих элементов
        for (size_t i = 0; i < size_; ++i) {
            alloc.construct(new_data + i, std::move_if_noexcept(*(data_ + i)));
            alloc.destroy(data_ + i);
        }
        
        if (data_) {
            alloc.deallocate(data_, capacity_);
        }
        data_ = new_data;
        capacity_ = new_capacity;
    }
};