//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_TYPES_H
#define OPTIMIZE_TYPES_H

enum Player { Decision, Parameter };

enum VariableType { Continuous, Binary, Integer };

template<enum Player> struct opp_player {};
template<> struct opp_player<Decision> { static constexpr Player value = Parameter; };
template<> struct opp_player<Parameter> { static constexpr Player value = Decision; };
template<enum Player PlayerT> static constexpr Player opp_player_v = opp_player<PlayerT>::value;

#endif //OPTIMIZE_TYPES_H
