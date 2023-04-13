//
// Created by henri on 13/04/23.
//

#ifndef IDOL_COVERCUTS_H
#define IDOL_COVERCUTS_H

#include "CuttingPlaneGenerator.h"
#include "modeling/models/Model.h"
#include "modeling/expressions/operations/operators.h"
#include "optimizers/solvers/gurobi/Gurobi.h"
#include "optimizers/callbacks/UserCutCallback.h"

class CoverCuts : public CuttingPlaneGenerator {

    void parse_ctr(const Model& t_model, const Ctr& t_ctr) {

        const auto ctr_type = t_model.get_ctr_type(t_ctr);

        if (ctr_type != LessOrEqual) { return; }

        auto& env = t_model.env();
        Model separation(env);
        const auto& row = t_model.get_ctr_row(t_ctr);
        double rhs = row.rhs().numerical();
        Expr lhs;
        std::list<Var> covered_vars;

        for (const auto& [var, constant] : row.linear()) {

            const auto var_type = t_model.get_var_type(var);

            if (var_type == Continuous) { return; }

            const double var_ub = t_model.get_var_ub(var);
            const double var_lb = t_model.get_var_lb(var);

            if (is_pos_inf(var_ub)) { return; }

            if (var_lb < -.5 || var_ub > 1.5) { return; }

            const double coeff = constant.numerical();

            if (coeff < 0) {
                rhs -= coeff * var_ub;
            } else {
                lhs += coeff * var;
                covered_vars.emplace_back(var);
                separation.add(var, TempVar(0, 1, Binary, Column()));
            }

        }

        separation.add_ctr(lhs >= rhs + 1);

        add_callback(
                UserCutCallback(separation, idol_Sum(var, covered_vars, !var * var) <= idol_Sum(var, covered_vars, !var) - 1)
                        .with_separation_solver(Gurobi())
        );
    }

public:

    void operator()(const Model &t_model) override {

        for (auto& ctr : t_model.ctrs()) {
            parse_ctr(t_model, ctr);
        }

    }

    [[nodiscard]] CuttingPlaneGenerator *clone() const override {
        return new CoverCuts(*this);
    }
};

#endif //IDOL_COVERCUTS_H
