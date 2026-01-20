#pragma once

#include "compressed_pair.h"

#include <cstddef>
#include <type_traits>

template <typename T>
struct Slug {
    Slug() = default;
    Slug(Slug&& rhs) noexcept = default;
    template <class U>
    Slug(Slug<U>&&) noexcept {
    }
    void operator()(T* ptr) const noexcept {
        delete ptr;
    }
    Slug& operator=(Slug&&) noexcept {
        return *this;
    }
    template <typename U>
    Slug& operator=(Slug<U>&&) noexcept {
        return *this;
    }
};

template <typename T>
struct Slug<T[]> {
    Slug() = default;
    Slug(Slug&& rhs) noexcept = default;
    template <class U>
    Slug(Slug<U[]>&&) noexcept {
    }

    void operator()(T* ptr) const noexcept {
        delete[] ptr;
    }
    Slug& operator=(Slug&&) noexcept {
        return *this;
    }
    template <typename U>
    Slug& operator=(Slug<U[]>&&) noexcept {
        return *this;
    }
};

template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
private:
    CompressedPair<T*, Deleter> holder_;

public:
    UniquePtr(T* ptr = nullptr) : holder_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : holder_(ptr, std::move(deleter)) {
    }

    UniquePtr(UniquePtr&& other) noexcept
        : holder_(other.Release(), std::forward<Deleter>(other.GetDeleter())) {
    }

    template <typename U, typename D>
    UniquePtr(UniquePtr<U, D>&& other) noexcept
        : holder_(other.Release(), std::forward<D>(other.GetDeleter())) {
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Reset(other.Release());
            GetDeleter() = std::forward<Deleter>(other.GetDeleter());
        }
        return *this;
    }

    template <typename U, typename D>
    UniquePtr& operator=(UniquePtr<U, D>&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::forward<D>(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    ~UniquePtr() {
        if (Get()) {
            GetDeleter()(Get());
        }
    }

    T* Release() noexcept {
        T* old = Get();
        holder_.GetFirst() = nullptr;
        return old;
    }

    void Reset(T* ptr = nullptr) noexcept {
        T* old = Get();
        holder_.GetFirst() = ptr;
        if (old && old != ptr) {
            GetDeleter()(old);
        }
    }

    void Swap(UniquePtr& other) noexcept {
        std::swap(holder_.GetFirst(), other.holder_.GetFirst());
        std::swap(GetDeleter(), other.GetDeleter());
    }

    T* Get() const noexcept {
        return holder_.GetFirst();
    }

    Deleter& GetDeleter() noexcept {
        return holder_.GetSecond();
    }

    const Deleter& GetDeleter() const noexcept {
        return holder_.GetSecond();
    }

    explicit operator bool() const noexcept {
        return Get() != nullptr;
    }

    std::add_lvalue_reference<T>::type operator*() const noexcept {
        return *Get();
    }

    T* operator->() const noexcept {
        return Get();
    }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
private:
    CompressedPair<T*, Deleter> holder_;

public:
    UniquePtr(T* ptr = nullptr) : holder_(ptr, Deleter()) {
    }

    UniquePtr(T* ptr, Deleter deleter) : holder_(ptr, std::move(deleter)) {
    }

    UniquePtr(UniquePtr&& other) noexcept
        : holder_(other.Release(), std::forward<Deleter>(other.GetDeleter())) {
    }

    ~UniquePtr() {
        if (Get()) {
            GetDeleter()(Get());
        }
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Reset(other.Release());
            GetDeleter() = std::forward<Deleter>(other.GetDeleter());
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    T* Release() noexcept {
        T* old = Get();
        holder_.GetFirst() = nullptr;
        return old;
    }

    void Reset(T* ptr = nullptr) noexcept {
        T* old = Get();
        holder_.GetFirst() = ptr;
        if (old && old != ptr) {
            GetDeleter()(old);
        }
    }

    void Swap(UniquePtr& other) noexcept {
        std::swap(holder_.GetFirst(), other.holder_.GetFirst());
        std::swap(GetDeleter(), other.GetDeleter());
    }

    T* Get() const noexcept {
        return holder_.GetFirst();
    }

    Deleter& GetDeleter() noexcept {
        return holder_.GetSecond();
    }

    const Deleter& GetDeleter() const noexcept {
        return holder_.GetSecond();
    }

    explicit operator bool() const noexcept {
        return Get() != nullptr;
    }

    T& operator[](size_t i) const noexcept {
        return Get()[i];
    }
};
