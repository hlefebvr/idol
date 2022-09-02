//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_SAFEPOINTER_H
#define OPTIMIZE_SAFEPOINTER_H

template<class T>
class SafePointer {
    T* m_ptr = nullptr;
public:
    SafePointer() = default;
    SafePointer(T* t_ptr) : m_ptr(t_ptr) {} // NOLINT(google-explicit-constructor)

    SafePointer(const SafePointer&) = default;
    SafePointer(SafePointer&&) noexcept = default;

    SafePointer& operator=(const SafePointer&) = default;
    SafePointer& operator=(SafePointer&&) noexcept = default;

    T& operator*() { return *m_ptr; }
    const T& operator*() const { return *m_ptr; }

    T* operator->() { return m_ptr; }
    const T* operator->() const { return m_ptr; }
};

#endif //OPTIMIZE_SAFEPOINTER_H
