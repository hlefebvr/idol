//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_SET_H
#define OPTIMIZE_SET_H

#include <unordered_set>

template<
        class Key,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator< Key >
>
using Set = std::unordered_set<Key, Hash, KeyEqual, Allocator>;
#endif //OPTIMIZE_SET_H
