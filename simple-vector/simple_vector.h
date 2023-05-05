#pragma once

#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include <iterator>
#include <utility>

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity_to_reserve) : capacity_(capacity_to_reserve){
    }
    size_t GetCapacity() const {
        return capacity_;
    }
private:
    size_t capacity_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    SimpleVector(SimpleVector&& other) noexcept :
             size_(std::exchange(other.size_, 0)),
             capacity_(std::exchange(other.capacity_, 0)),
             items_(std::move(other.items_)){
    }


    SimpleVector& operator=(SimpleVector&& rhs)  noexcept{
        if(*this != rhs){
            SimpleVector tmp(std::move(rhs));
            swap(tmp);
        }
        return *this;
    }

    SimpleVector(const SimpleVector& other) : size_(other.GetSize()), capacity_(other.GetCapacity()), items_(other.GetSize()){
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector& operator=(const SimpleVector& rhs){
        if (*this != rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : size_(size), capacity_(size), items_(size)
    {
        std::generate(items_.Get(), items_.Get()+size, []{return Type{};});
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size), items_(size){
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()), items_(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }

    explicit SimpleVector(ReserveProxyObj obj){
        Reserve(obj.GetCapacity());
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index <= size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index <= size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size_){
            throw std::out_of_range("out_of_range\n");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if(index >= size_){
            throw std::out_of_range("out_of_range\n");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if(new_size == size_) {
            return;
        }
        if(new_size < size_){
            size_ = new_size;
        }
        else if(new_size <= capacity_){
            for (size_t i = size_; i < new_size; ++i)
            {
                items_[i] = Type{};
            }
            size_ = new_size;
        }
        else{
            capacity_ = std::max(capacity_ * 2, new_size);
            SimpleVector<Type> tmp(capacity_);
            for (size_t i = 0; i < size_; ++i) {
                tmp.items_[i] = std::move(items_[i]);
            }
            swap(tmp);
            size_ = new_size;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return Iterator{items_.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return Iterator{items_.Get() + size_};
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return ConstIterator{items_.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return ConstIterator {items_.Get() + size_};
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return ConstIterator{items_.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return ConstIterator{items_.Get() + size_};
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if(size_ < capacity_){
            ++size_;
        }
        else{
            Resize(size_ + 1);
        }
        items_[size_- 1] = item;
    }

    void PushBack(Type&& item) {
        if(size_ < capacity_){
            ++size_;
        }
        else{
            Resize(size_ + 1);
        }
        items_[size_- 1] = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= cbegin() && pos <= cend());
        if(pos == cend()){
            PushBack(std::move(value));
            return (end() - 1);
        }
        if(size_ < capacity_){
            ++size_;
        }
        else{
            Resize(size_ + 1);
        }
        size_t index = std::distance(cbegin(), pos);
        Iterator it = std::move_backward(&items_[index], end() - 1, end());
        *(--it) = value;
        return it;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= cbegin() && pos <= cend());
        if(pos == cend()){
            PushBack(std::move(value));
            return (end() - 1);
        }
        if(size_ < capacity_){
            ++size_;
        }
        else{
            Resize(size_ + 1);
        }
        size_t index = std::distance(cbegin(), pos);
        Iterator it = std::move_backward(&items_[index], end() - 1, end());
        *(--it) = std::move(value);
        return it;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(!IsEmpty());
        assert(pos >= cbegin() && pos < cend());
        size_t index = std::distance(cbegin(), pos);
        std::move(&items_[index + 1], end(), &items_[index]);
        --size_;
        return &items_[index];
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void Reserve(size_t new_capacity){
        if(new_capacity > capacity_){
            ArrayPtr<Type> new_items(new_capacity);
            std::copy(begin(), end(), new_items.Get());
            items_.swap(new_items);
            capacity_ = new_capacity;
        }
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> items_ = {};
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
