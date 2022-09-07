//
// Created by henri on 06/09/22.
//

#ifndef OPTIMIZE_POINTERS_H
#define OPTIMIZE_POINTERS_H

template<class T>
class Pointer {
    T* m_ptr = nullptr;
public:
    Pointer() = default;
    Pointer(T* t_ptr) : m_ptr(t_ptr) {} // NOLINT(google-explicit-constructor)
    Pointer(const Pointer& t_ptr) : m_ptr(new T(*t_ptr)) {}
    Pointer(Pointer&& t_src)  noexcept : m_ptr(t_src.m_ptr) { t_src.m_ptr = nullptr; }
    Pointer& operator=(const Pointer& t_src) noexcept {
        if (this == &t_src) { return *this; }
        delete m_ptr;
        m_ptr = new T(t_src.m_ptr);
        return *this;
    }
    Pointer& operator=(Pointer&& t_src) noexcept {
        if (this == &t_src) { return *this; }
        delete m_ptr;
        m_ptr = t_src.m_ptr;
        t_src.m_ptr = nullptr;
        return *this;
    }
    ~Pointer() { delete m_ptr; m_ptr = nullptr; }
    T& operator*() { return *m_ptr; }
    const T& operator*() const { return *m_ptr; }
    T* operator->() { return m_ptr; }
    const T* operator->() const { return m_ptr; }
};

template<class T>
class FreePointer {
    T* m_ptr = nullptr;
public:
    FreePointer() = default;
    FreePointer(T* t_ptr) : m_ptr(t_ptr) {} // NOLINT(google-explicit-constructor)
    FreePointer(const FreePointer& t_ptr) : m_ptr(new T(*t_ptr)) {}
    FreePointer(FreePointer&& t_src)  noexcept : m_ptr(t_src.m_ptr) { t_src.m_ptr = nullptr; }
    FreePointer& operator=(const FreePointer& t_src) noexcept {
        if (this == &t_src) { return *this; }
        delete m_ptr;
        m_ptr = new T(t_src.m_ptr);
        return *this;
    }
    FreePointer& operator=(FreePointer&& t_src) noexcept {
        if (this == &t_src) { return *this; }
        delete m_ptr;
        m_ptr = t_src.m_ptr;
        t_src.m_ptr = nullptr;
        return *this;
    }
    T& operator*() { return *m_ptr; }
    const T& operator*() const { return *m_ptr; }
    T* operator->() { return m_ptr; }
    const T* operator->() const { return m_ptr; }
};



#endif //OPTIMIZE_POINTERS_H
