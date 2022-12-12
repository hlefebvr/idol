//
// Created by henri on 16/10/22.
//

#ifndef IDOL_VECTOR_H
#define IDOL_VECTOR_H

#include <array>
#include <vector>
#include <functional>

template<int N>
class Dim : public std::array<unsigned int, N> {
    friend class Dim<N+1>;
public:
    explicit Dim(std::array<unsigned int, N> t_dims) : std::array<unsigned int, N>(std::move(t_dims)) {}
    template<class ...ArgsT> explicit Dim(ArgsT&& ...t_args) : std::array<unsigned int, N>({ (unsigned int) t_args... }) {
        static_assert(sizeof...(ArgsT) == N);
    }
};

namespace impl {

    template<class T, unsigned int N>
    struct Vector {
        using type = std::vector<typename Vector<T, N - 1>::type>;
    };

    template<class T>
    struct Vector<T, 0> {
        using type = T;
    };

}

template<class T, unsigned int N = 1> using Vector = typename impl::Vector<T, N>::type;

template<class T, unsigned int Size>
unsigned int n_entries(const Vector<T, Size>& t_vector) {

    if constexpr (Size == 1) {

        return t_vector.size();

    } else {

        unsigned int result = 0;
        for (const auto& vec : t_vector) {
            result += n_entries<T, Size - 1>(vec);
        }

        return result;
    }

}

template<class T, unsigned int Size>
void append(std::vector<T>& t_dest, const Vector<T, Size>& t_vector) {
    if constexpr (Size == 1) {

        for (const T& elem : t_vector) {
            t_dest.emplace_back(elem);
        }

    } else {

        for (const auto& vec : t_vector) {
            append(t_dest, vec);
        }

    }
}

template<class T, unsigned int Size>
std::vector<T> flatten(const Vector<T, Size>& t_vector) {

    if constexpr (Size == 1) {

        return t_vector;

    } else {

        std::vector<T> result;
        result.reserve(n_entries<T, Size>(t_vector));

        for (const auto &vec: t_vector) {
            append<T, Size - 1>(result, vec);
        }

        return result;
    }
}

template<class T, unsigned int Size>
void apply(Vector<T, Size>& t_vec, const std::function<void(T&)>& t_function) {

    if constexpr (Size == 1) {

        for (auto& elem : t_vec) {
            t_function(elem);
        }

    } else {

        for (auto& elem : t_vec) {
            apply<T, Size - 1>(elem, t_function);
        }

    }

}


template<class T, unsigned int Size>
void apply(const Vector<T, Size>& t_vec, const std::function<void(const T&)>& t_function) {

    if constexpr (Size == 1) {

        for (const auto& elem : t_vec) {
            t_function(elem);
        }

    } else {

        for (const auto& elem : t_vec) {
            apply<T, Size - 1>(elem, t_function);
        }

    }

}

template<class T, class U, unsigned int Size>
Vector<U, Size> transform(const Vector<T, Size>& t_vec, const std::function<U(const T&)>& t_function) {

    Vector<U, Size> result;
    result.reserve(t_vec.size());

    if constexpr (Size == 1) {

        for (const auto& elem : t_vec) {
            result.emplace_back(t_function(elem));
        }

    } else {

        for (const auto& elem : t_vec) {
            result.emplace_back(transform<T, U, Size - 1>(elem, t_function));
        }

    }

    return result;

}

#endif //IDOL_VECTOR_H
