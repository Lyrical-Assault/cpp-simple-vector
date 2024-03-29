#pragma once

#include <cassert>
#include <cstdlib>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    ArrayPtr(ArrayPtr&& other) noexcept {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        return *this;
    }

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        if(size == 0){
            raw_ptr_ = nullptr;
        }
        else{
            Type* arr = new Type[size];
            raw_ptr_ = arr;
        }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        if(raw_ptr != nullptr){
            raw_ptr_ = raw_ptr;
        }
        else{
            raw_ptr_ = nullptr;
        }
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }


    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        Type* tmp = raw_ptr_;
        raw_ptr_ = nullptr;
        return tmp;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        auto tmp = raw_ptr_;
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = tmp;
    }

private:
    Type* raw_ptr_ = nullptr;
};
