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

/*
#include <map>

template<
        class Key,
        class T,
        class Compare = std::less<Key>,
        class Allocator = std::allocator< std::pair<const Key, T> >
>
using Map = std::map<Key, T, Compare, Allocator>;
*/
/*
#include <robin_hood/robin_hood.h>

template<
        class Key,
        class T,
        class Hash = robin_hood::hash<Key>,
        class KeyEqual = std::equal_to<Key>
>
using Map = robin_hood::unordered_map<Key, T, Hash, KeyEqual>;
*/

#endif //OPTIMIZE_MAP_H
