//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_MAP_H
#define OPTIMIZE_MAP_H

#include <unordered_map>

template<
        class Key,
        class T,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator< std::pair<const Key, T> >
>
using Map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

#endif //OPTIMIZE_MAP_H
