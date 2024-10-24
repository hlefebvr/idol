//
// Created by henri on 15.11.23.
//

#ifndef IDOL_PAIR_H
#define IDOL_PAIR_H

#include <iostream>

namespace idol {
    template<class T1, class T2> struct Pair;
}

template<class T1, class T2>
struct idol::Pair {
    T1 first;
    T2 second;
    Pair(const T1& t_1, const T2& t_2) : first(t_1), second(t_2) {}

    Pair(const Pair&) = default;
    Pair(Pair&&) = default;

    Pair& operator=(const Pair&) = default;
    Pair& operator=(Pair&&) = default;

    bool operator==(const Pair& t_rhs) const {
        return std::equal_to<T1>()(first, t_rhs.first) && std::equal_to<T2>()(second, t_rhs.second);
    }
};

namespace idol {
    template<class T1, class T2>
    static std::ostream &operator<<(std::ostream &t_os, const Pair<T1, T2> &t_pair) {
        return t_os << t_pair.first << " " << t_pair.second;
    }
}

#endif //IDOL_PAIR_H
