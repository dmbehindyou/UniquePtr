#pragma once

#include <utility>

class EmptyPair {};

// Me think, why waste time write lot code, when few code do trick.
template <typename T, int num, bool b = std::is_empty_v<T> && !std::is_final_v<T>>
class Element;

template <typename T, int num>
class Element<T, num, true> : T {
public:
    Element() = default;
    template <typename U>
    Element(U&& val) : T(std::forward<U>(val)) {
    }
    Element(const T& val) : T(val) {
    }
    T& Get() {
        return *this;
    }
    const T& Get() const {
        return *this;
    }
};

template <typename T, int num>
class Element<T, num, false> {
public:
    T val_;
    Element() : val_{} {
    }
    template <typename U>
    Element(U&& val) : val_(std::forward<U>(val)) {
    }
    Element(const T& val) : val_(val) {
    }
    T& Get() {
        return val_;
    }
    const T& Get() const {
        return val_;
    }
};

template <typename F, typename S>
class CompressedPair : Element<F, 0>, Element<S, 1> {
public:
    CompressedPair() = default;
    template <typename UF, typename US>
    CompressedPair(UF&& first, US&& second)
        : Element<F, 0>(std::forward<UF>(first)), Element<S, 1>(std::forward<US>(second)) {
    }

    F& GetFirst() {
        return static_cast<Element<F, 0>&>(*this).Get();
    }

    S& GetSecond() {
        return static_cast<Element<S, 1>&>(*this).Get();
    }

    const F& GetFirst() const {
        return static_cast<const Element<F, 0>&>(*this).Get();
    }

    const S& GetSecond() const {
        return static_cast<const Element<S, 1>&>(*this).Get();
    }
};
