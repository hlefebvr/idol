//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_OPTIONAL_H
#define OPTIMIZE_OPTIONAL_H

#include <optional>

template<class T> using Optional = std::optional<T>;
template<class T> static constexpr Optional<T> skip = std::optional<T>();

#endif //OPTIMIZE_OPTIONAL_H
