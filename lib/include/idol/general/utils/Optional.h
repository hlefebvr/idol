//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_OPTIONAL_H
#define OPTIMIZE_OPTIONAL_H

#include <optional>

namespace idol {

    template<class T>
    class Optional {
        T* m_value = nullptr;
    public:
        Optional() = default;

        Optional(const Optional<T>& t_optional) : m_value(t_optional.m_value ? new T(*t_optional.m_value) : nullptr) {}

        Optional(Optional<T>&& t_optional) noexcept : m_value(t_optional.m_value) {
            t_optional.m_value = nullptr;
        }

        Optional(const T& t_value) : m_value(new T(t_value)) {}

        Optional(T&& t_value) : m_value(new T(std::move(t_value))) {}

        ~Optional() {
            if (m_value) {
                delete m_value;
            }
        }
        
        bool has_value() const { return m_value; }

        T& value() {
            if (!m_value) {
                throw std::bad_optional_access();
            }
            return *m_value;
        }

        const T& value() const {
            if (!m_value) {
                throw std::bad_optional_access();
            }
            return *m_value;
        }

        T& operator*() { return value(); }

        const T& operator*() const { return value(); }

        T* operator->() { return m_value; }

        const T* operator->() const { return m_value; }

        explicit operator bool() const { return m_value; }

        Optional<T>& operator=(T&& t_value) {
            if (m_value) {
                delete m_value;
            }
            m_value = new T(std::move(t_value));
            return *this;
        }

        Optional<T>& operator=(const T& t_value) {
            if (m_value) {
                delete m_value;
            }
            m_value = new T(t_value);
            return *this;
        }

        void reset() {
            if (m_value) {
                delete m_value;
                m_value = nullptr;
            }
        }

        template<class ...ArgsT>
        T& emplace(ArgsT&& ...t_args) {
            if (m_value) {
                delete m_value;
            }
            m_value = new T(std::forward<ArgsT>(t_args)...);
            return *m_value;
        }

    };

    template<class T, class ...ArgsT>
    Optional<T> make_optional(ArgsT&& ...t_args) {
        Optional<T> optional;
        optional.emplace(std::forward<ArgsT>(t_args)...);
        return optional;
    }
}

#endif //OPTIMIZE_OPTIONAL_H
