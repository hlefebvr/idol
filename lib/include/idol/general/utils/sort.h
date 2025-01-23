//
// Created by henri on 24.10.24.
//

#ifndef IDOL_SORT_H
#define IDOL_SORT_H

#include <vector>
#include <numeric>
#include <functional>
#include <iostream>
#include "idol/general/utils/Pair.h"

namespace idol {
    template<class T> struct get_id;

    template<class Key1, class Key2>
    struct get_id<std::pair<Key1, Key2>> {
        auto operator()(const std::pair<Key1, Key2>& t) const { return std::make_pair(get_id<Key1>()(t.first), get_id<Key2>()(t.second)); }
    };

    template<class T>
    struct get_id {
        unsigned int operator()(const T& t) const { return t.id(); }
    };

    template<class Key1, class Key2>
    struct get_id<idol::Pair<Key1, Key2>> {
        auto operator()(const idol::Pair<Key1, Key2>& t) const { return std::make_pair(get_id<Key1>()(t.first), get_id<Key2>()(t.second)); }
    };

    template<class T>
    struct identity {
        const T& operator()(const T& t) const { return t; }
    };

    template<class T>
    void apply_permutation(const std::vector<unsigned int>& t_permutation, std::vector<T>& t_arg) {

        std::vector<bool> visited(t_arg.size(), false);  // To track visited elements

        for (size_t i = 0; i < t_arg.size(); ++i) {

            if (visited[i]) {
                continue;  // Skip if already processed
            }

            // Apply permutation cycle starting at index i
            size_t current = i;
            T temp = std::move(t_arg[i]);  // Store the element to be moved

            while (true) {
                size_t next = t_permutation[current];  // Find the target index
                if (next == i) {
                    break;  // End of cycle
                }

                t_arg[current] = std::move(t_arg[next]);  // Move the element
                visited[current] = true;  // Mark as visited
                current = next;  // Move to the next index
            }

            t_arg[current] = std::move(temp);  // Place the stored element in its final position
            visited[current] = true;  // Mark the last element as visited
        }
    }

    template<class FirstT, class ...T>
    void apply_permutation(const std::vector<unsigned int>& t_permutation, std::vector<FirstT>& t_first, std::vector<T>& ...t_args) {
        apply_permutation(t_permutation, t_first);
        apply_permutation(t_permutation, t_args...);
    }

    template<class IndexT, class IndexExtractor = identity<IndexT>, class ...T>
    void sort(std::vector<IndexT>& t_index,
              std::vector<T>& ... t_args) {

        std::vector<unsigned int> permutation(t_index.size());
        std::iota(permutation.begin(), permutation.end(), 0);
        std::sort(
            permutation.begin(),
            permutation.end(),
            [&t_index, extractor = IndexExtractor()](unsigned int i, unsigned int j) {
                return extractor(t_index[i]) < extractor(t_index[j]);
            }
        );

        apply_permutation(permutation, t_index, t_args...);

    }
}

#endif //IDOL_SORT_H
