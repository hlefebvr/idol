//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_INSTANCE_H
#define OPTIMIZE_INSTANCE_H

#include "modeling.h"

class Instance {
public:
    const unsigned int n_knapsacks = 3;
    const unsigned int n_items = 8;

    const std::vector<std::vector<double>> p = {
            { 27, 12, 12, 16, 24, 31, 41, 13 },
            { 14,  5, 37,  9, 36, 25,  1, 35 },
            { 34, 34, 20,  9, 19, 19,  3, 24 }
    };

    const std::vector<std::vector<double>> w = {
            { 21, 13,  9,  5,  7, 15,  5, 24 },
            { 20,  8, 18, 25,  6,  6,  9,  6 },
            { 16, 16, 18, 24, 11, 11, 16, 18 }
    };

    const std::vector<double> c = { 26, 25, 34 };

    std::vector<std::vector<Var>> create_variables(Model& t_model, VarType t_type) const {

        std::vector<std::vector<Var>> result(n_knapsacks);

        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            result[i].reserve(n_items);
            for (unsigned int j = 0 ; j < n_items ; ++j) {
                result[i].emplace_back( t_model.add_variable(0., 1., t_type, p[i][j], "x(" + std::to_string(i) + "," + std::to_string(j) + ")") );
            }
        };

        return result;
    }

    std::vector<Ctr> create_knapsack_constraints(Model& t_model, const std::vector<std::vector<Var>>& t_x) const {

        std::vector<Ctr> result;
        result.reserve(n_knapsacks);

        for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
            Expr expr;
            for (unsigned int j = 0 ; j < n_items ; ++j) {
                expr += w[i][j] * t_x[i][j];
            }
            result.emplace_back( t_model.add_constraint(expr <= c[i]) );
        }

        return result;

    }

    std::vector<Ctr> create_assignment_constraints(Model& t_model, const std::vector<std::vector<Var>>& t_x) const {

        std::vector<Ctr> result;
        result.reserve(n_items);

        for (unsigned int j = 0 ; j < n_items ; ++j) {
            Expr expr;
            for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
                expr += t_x[i][j];
            }
            result.emplace_back( t_model.add_constraint(expr == 1) );
        }

        return result;
    }
};


#endif //OPTIMIZE_INSTANCE_H
