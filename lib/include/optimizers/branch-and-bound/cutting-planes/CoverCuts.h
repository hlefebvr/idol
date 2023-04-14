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

    bool m_adaptivity = false;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    void parse_ctr(const Model& t_model, const Ctr& t_ctr) {

        if (!m_optimizer_factory) {
            throw Exception("No optimizer factory was given.");
        }

        const auto ctr_type = t_model.get_ctr_type(t_ctr);

        if (ctr_type != LessOrEqual) { return; }

        auto& env = t_model.env();
        Model separation(env);
        const auto& row = t_model.get_ctr_row(t_ctr);
        double rhs = row.rhs().numerical();
        Expr lhs;
        std::list<Var> covered_vars;

        std::optional<Var> adaptive_var;
        bool use_adaptive_var = m_adaptivity;

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

                if (adaptive_var.has_value()) {
                    use_adaptive_var = false;
                } else {
                    adaptive_var = var;
                }

            } else {
                lhs += coeff * var;
                covered_vars.emplace_back(var);
                separation.add(var, TempVar(0, 1, Binary, Column()));
            }

        }

        if (rhs <= 1) { return; }

        separation.add_ctr(lhs >= rhs + 1);


        Expr cut_rhs;
        if (use_adaptive_var && adaptive_var.has_value()) {
            cut_rhs = (idol_Sum(var, covered_vars, !var) - 1).constant() * adaptive_var.value();
        } else {
            cut_rhs = idol_Sum(var, covered_vars, !var) - 1;
        }

        //std::cout << "Adding separation for " << row << std::endl;

        add_callback(
                UserCutCallback(separation, idol_Sum(var, covered_vars, !var * var) <= std::move(cut_rhs))
                        .with_separation_solver(*m_optimizer_factory)
        );
    }

    CoverCuts(const CoverCuts& t_src)
        : m_adaptivity(t_src.m_adaptivity),
          m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr) {}

public:
    CoverCuts() = default;

    void operator()(const Model &t_model) override {

        for (auto& ctr : t_model.ctrs()) {
            parse_ctr(t_model, ctr);
        }

    }

    [[nodiscard]] CuttingPlaneGenerator *clone() const override {
        return new CoverCuts(*this);
    }

    CoverCuts& with_solver(const OptimizerFactory& t_optimizer_factory) {
        m_optimizer_factory.reset(t_optimizer_factory.clone());
        return *this;
    }

    CoverCuts& with_adaptivity(bool t_value) {
        m_adaptivity = t_value;
        return *this;
    }
};

#endif //IDOL_COVERCUTS_H
