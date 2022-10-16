//
// Created by henri on 16/10/22.
//

#ifndef IDOL_VECTOR_H
#define IDOL_VECTOR_H

#include <array>
#include <vector>

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

    template<class T, int N>
    struct Vector {
        using type = std::vector<typename Vector<T, N - 1>::type>;
    };

    template<class T>
    struct Vector<T, 0> {
        using type = T;
    };

}

template<class T, int N = 1> using Vector = typename impl::Vector<T, N>::type;

#endif //IDOL_VECTOR_H
