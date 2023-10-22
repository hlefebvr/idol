//
// Created by henri on 22/09/22.
//

#ifndef OPTIMIZE_CARTESIAN_PRODUCT_H
#define OPTIMIZE_CARTESIAN_PRODUCT_H

#include <functional>
#include <tuple>
#include <utility>

namespace test_impl {

    // Thanks to https://codereview.stackexchange.com/questions/217452/a-cartesian-product-of-tuples-in-c17
    // The accepted answer has been modified to work with non-copyable objects (but movable).

    constexpr std::tuple<std::tuple<>> cartesian_product() {
        return {{}};
    }

    template<class X, class ... Y>
    constexpr auto cartesian_product(X &&tx, Y &&... ty) {
        if constexpr (std::tuple_size_v<X> == 0) return std::make_tuple();
        else
            return std::apply([&](auto &&... tuples) {
                return std::apply([&](auto &&... xxs) {
                    const auto recursive = [&](
                            const auto &self,
                            auto &&x, auto &&... xs) {
                        auto tuple = std::make_tuple(
                                std::tuple_cat(std::make_tuple(std::forward<decltype(x)>(x)), std::move(tuples))...);
                        if constexpr (sizeof...(xs) == 0) return tuple;
                        else return std::tuple_cat(std::move(tuple), self(self, std::move(xs)...));
                    };
                    return recursive(recursive, std::move(xxs)...);
                }, tx);
            }, cartesian_product(std::move(ty)...));
    }

}

template<class ...T>
using cartesian_product = decltype(test_impl::cartesian_product( std::declval<T>()...));


#endif //OPTIMIZE_CARTESIAN_PRODUCT_H
