//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_TEST_UTILS_H
#define OPTIMIZE_TEST_UTILS_H

#include "modeling.h"

#include <catch2/catch_all.hpp>

using namespace Catch::literals;

template<enum Player GenPlayerT> struct type {
    static constexpr Player PlayerT = GenPlayerT;
    static constexpr Player OppositePlayerT = opp_player_v<GenPlayerT>;
};

using for_each_player = std::tuple<type<Decision>, type<Parameter>>;


#endif //OPTIMIZE_TEST_UTILS_H
