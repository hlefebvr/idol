//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_MAP_H
#define OPTIMIZE_MAP_H


#ifdef IDOL_USE_ROBINHOOD
#include <robin_hood.h>

#else
#include <unordered_map>
#endif

#include "Pair.h"

// Implements hash for pairs (non-symmetric by default (std::hash<idol::Pair<T, U>>) and symmetric impls)
// See https://youngforest.github.io/2020/05/27/best-implement-to-use-pair-as-key-to-std-unordered-map-in-C/
namespace idol::impl {

#ifdef IDOL_USE_ROBINHOOD
    template<class T, class EnableT = void> using hash = robin_hood::hash<T, EnableT>;
#else
    template<class A> using hash = std::hash<A>;
#endif

    template <typename T>
    inline void hash_combine(std::size_t &seed, const T &val) {
        seed ^= hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    // auxiliary generic functions to create a hash value using a seed
    template <typename T> inline void hash_val(std::size_t &seed, const T &val) {
        hash_combine(seed, val);
    }

    template <typename T, typename... Types>
    inline void hash_val(std::size_t &seed, const T &val, const Types &... args) {
        hash_combine(seed, val);
        hash_val(seed, args...);
    }

    template <typename... Types>
    inline std::size_t hash_val(const Types &... args) {
        std::size_t seed = 0;
        hash_val(seed, args...);
        return seed;
    }

    struct symmetric_pair_hash {
        template<class Key>
        std::size_t operator()(const idol::Pair<Key, Key> &t_pair) const {
            return std::less<Key>()(t_pair.first, t_pair.second) ?
                   hash<idol::Pair<Key, Key>>()(t_pair)
                 : hash<idol::Pair<Key, Key>>()(idol::Pair(t_pair.second, t_pair.first));
        }
    };

    struct symmetric_pair_equal_to {
        template<class Key>
        std::size_t operator()(const idol::Pair<Key, Key>& t_a, const idol::Pair<Key, Key>& t_b) const {
            const auto a = std::less<Key>()(t_a.first, t_a.second) ? t_a : idol::Pair(t_a.second, t_a.first);
            const auto b = std::less<Key>()(t_b.first, t_b.second) ? t_b : idol::Pair(t_b.second, t_b.first);
            return std::equal_to<Key>()(a.first, b.first) && std::equal_to<Key>()(a.second, b.second);
        }
    };

}

template<class Key1, class Key2>
struct std::hash<idol::Pair<Key1, Key2>> {
    std::size_t operator()(const idol::Pair<Key1, Key2>& t_pair) const {
        return idol::impl::hash_val(t_pair.first, t_pair.second);
    }
};

#ifdef IDOL_USE_ROBINHOOD

namespace idol {

    template<
            class Key,
            class T,
            class Hash = robin_hood::hash<Key>,
            class KeyEqual = std::equal_to<Key>
    >
    using Map = robin_hood::unordered_flat_map<Key, T, Hash, KeyEqual>;

}

#else

namespace idol {

    template<
            class Key,
            class T,
            class Hash = std::hash<Key>,
            class KeyEqual = std::equal_to<Key>,
            class Allocator = std::allocator<std::pair<const Key, T> >
    >
    using Map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

}

#endif

#endif //OPTIMIZE_MAP_H
