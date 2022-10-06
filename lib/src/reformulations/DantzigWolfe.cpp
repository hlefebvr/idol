//
// Created by henri on 06/10/22.
//
#include "reformulations/DantzigWolfe.h"

Model DantzigWolfe::reformulate(Model &t_rmp, const std::function<bool(const Ctr&)>& t_complicating_constraints_indicator) {

    Model result;

    Map<Ctr, Expr<Var>> generator;

    for (const auto& var : t_rmp.variables()) {

        for (const auto& [ctr, coeff] : var.column().components()) {

            if (!t_complicating_constraints_indicator(ctr)) {
                auto [it, success] = generator.emplace(ctr, coeff * var);
                if (!success) {
                    it->second += coeff * var;
                }
            }

        }

    }

    for (const auto& [ctr, expr] : generator) {
        std::cout << ctr.name() << " -> " << expr << std::endl;
    }

    return result;
}
