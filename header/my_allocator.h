#pragma once

#include <memory>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <algorithm>

template <typename T, size_t BlockSize = 10>
class my_allocator {
private:
    // Внутренняя структура для хранения блоков памяти
    struct memory_block {
        std::unique_ptr<std::byte[]> data;
        std::vector<bool> allocated;
        
        memory_block(size_t size) : data(std::make_unique<std::byte[]>(size * sizeof(T))), allocated(size, false) {}
        
        T* get_element_ptr(size_t index) {
            return reinterpret_cast<T*>(data.get() + index * sizeof(T));
        }
    };
    
    std::vector<memory_block> blocks;
    size_t total_size;
    size_t used_size;

public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    // Конструктор по умолчанию
    my_allocator() noexcept : total_size(0), used_size(0) {
        reserve(BlockSize);
    }

    // Копирующий конструктор для поддержки rebind
    template <typename U, size_t OtherBlockSize>
    my_allocator(const my_allocator<U, OtherBlockSize>&) noexcept : total_size(0), used_size(0) {
        reserve(BlockSize);
    }

    // Выделение памяти для n элементов
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        
        // При необходимости расширяем память
        if (used_size + n > total_size) {
            reserve(total_size + ((n + BlockSize - 1) / BlockSize) * BlockSize);
        }

        // Ищем n последовательных свободных элементов
        for (size_type start = 0; start <= total_size - n; ++start) {
            bool found = true;
            for (size_type i = 0; i < n; ++i) {
                if (get_allocated(start + i)) {
                    found = false;
                    start += i; // Пропускаем занятые элементы
                    break;
                }
            }
            if (found) {
                // Помечаем элементы как занятые
                for (size_type i = 0; i < n; ++i) {
                    set_allocated(start + i, true);
                }
                used_size += n;
                return get_pointer(start);
            }
        }

        // Если не нашли последовательные свободные элементы, расширяемся
        size_type start = total_size;
        reserve(total_size + ((n + BlockSize - 1) / BlockSize) * BlockSize);
        for (size_type i = 0; i < n; ++i) {
            set_allocated(start + i, true);
        }
        used_size += n;
        return get_pointer(start);
    }

    // Освобождение памяти
    void deallocate(pointer p, size_type n) noexcept {
        if (p == nullptr || n == 0) return;
        
        size_type pos = get_position(p);
        if (pos != size_type(-1) && pos + n <= total_size) {
            for (size_type i = 0; i < n; ++i) {
                set_allocated(pos + i, false);
            }
            used_size -= n;
        }
    }

    // Конструирование объекта
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    // Разрушение объекта
    template <typename U>
    void destroy(U* p) {
        p->~U();
    }

    // Переопределение типа для других типов (rebind)
    template <typename U>
    struct rebind {
        using other = my_allocator<U, BlockSize>;
    };

private:
    // Резервирование памяти
    void reserve(size_type new_size) {
        if (new_size <= total_size) return;
        
        // Рассчитываем количество новых элементов
        size_type elements_to_add = new_size - total_size;
        size_type blocks_to_add = (elements_to_add + BlockSize - 1) / BlockSize;
        
        for (size_type i = 0; i < blocks_to_add; ++i) {
            blocks.emplace_back(BlockSize);
        }
        total_size = blocks.size() * BlockSize;
    }
    
    // Вспомогательные методы для работы с флагами выделения
    bool get_allocated(size_type pos) const {
        if (pos >= total_size) return false;
        size_t block_idx = pos / BlockSize;
        size_t elem_idx = pos % BlockSize;
        return blocks[block_idx].allocated[elem_idx];
    }
    
    void set_allocated(size_type pos, bool value) {
        if (pos >= total_size) return;
        size_t block_idx = pos / BlockSize;
        size_t elem_idx = pos % BlockSize;
        blocks[block_idx].allocated[elem_idx] = value;
    }
    
    // Получение указателя по позиции
    pointer get_pointer(size_type pos) {
        if (pos >= total_size) {
            throw std::out_of_range("Position out of range");
        }
        
        // Определяем блок и смещение
        size_t block_idx = pos / BlockSize;
        size_t elem_idx = pos % BlockSize;
        
        return blocks[block_idx].get_element_ptr(elem_idx);
    }
    
    // Получение позиции по указателю
    size_type get_position(pointer p) {
        std::byte* raw_ptr = reinterpret_cast<std::byte*>(p);
        
        for (size_t block_idx = 0; block_idx < blocks.size(); ++block_idx) {
            std::byte* block_start = blocks[block_idx].data.get();
            std::byte* block_end = block_start + BlockSize * sizeof(T);
            
            if (raw_ptr >= block_start && raw_ptr < block_end) {
                size_t offset = raw_ptr - block_start;
                if (offset % sizeof(T) == 0) {
                    return block_idx * BlockSize + (offset / sizeof(T));
                }
            }
        }
        return size_type(-1); // Не найден
    }
};

// Операторы сравнения
template <typename T1, size_t B1, typename T2, size_t B2>
bool operator==(const my_allocator<T1, B1>&, const my_allocator<T2, B2>&) noexcept {
    return true;
}

template <typename T1, size_t B1, typename T2, size_t B2>
bool operator!=(const my_allocator<T1, B1>&, const my_allocator<T2, B2>&) noexcept {
    return false;
}