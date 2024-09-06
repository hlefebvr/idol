//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_OPTIONAL_H
#define OPTIMIZE_OPTIONAL_H

#include <optional>

namespace idol {

    //template<class T> using Optional = std::optional<T>;
    //template<class T> static constexpr Optional<T> skip = std::optional<T>();

    template<class T>
    class Optional {
        T* m_value = nullptr;
    public:
        Optional() = default;

        ~Optional() {
            if (m_value) {
                delete m_value;
            }
        }

        bool has_value() const { return !m_value; }

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

        T& operator=(T&& t_value) {
            if (m_value) {
                delete m_value;
            }
            m_value = new T(std::move(t_value));
            return *m_value;
        }

        T& operator=(const T& t_value) {
            if (m_value) {
                delete m_value;
            }
            m_value = new T(t_value);
            return *m_value;
        }

        void reset() {
            if (m_value) {
                delete m_value;
                m_value = nullptr;
            }
        }

        void swap(Optional& t_other) {
            std::swap(m_value, t_other.m_value);
        }

        template<class ...ArgsT>
        T& emplace(ArgsT&& ...t_args) {
            if (m_value) {
                delete m_value;
            }
            m_value = new T(std::forward<ArgsT>(t_args)...);
            return *m_value;
        }

        void swap(Optional& t_lhs, Optional& t_rhs) {
            t_lhs.swap(t_rhs);
        }

        void swap(Optional&& t_lhs, Optional&& t_rhs) {
            t_lhs.swap(t_rhs);
        }

        void swap(Optional& t_lhs, Optional&& t_rhs) {
            t_lhs.swap(t_rhs);
        }

        void swap(Optional&& t_lhs, Optional& t_rhs) {
            t_lhs.swap(t_rhs);
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
