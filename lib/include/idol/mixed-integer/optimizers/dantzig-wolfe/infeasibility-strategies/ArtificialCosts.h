//
// Created by henri on 31.10.23.
//

#ifndef IDOL_ARTIFICIALCOSTS_H
#define IDOL_ARTIFICIALCOSTS_H

#include "DantzigWolfeInfeasibilityStrategy.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/ColumnGeneration.h"

namespace idol::DantzigWolfe {
    class ArtificialCosts;
}

class idol::DantzigWolfe::ArtificialCosts : public idol::DantzigWolfe::InfeasibilityStrategyFactory {
    std::optional<double> m_initial_costs;
    std::optional<double> m_update_factor;
    std::optional<unsigned int> m_max_updates_before_phase_I;
public:

    class Strategy : public InfeasibilityStrategyFactory::Strategy {
        double m_initial_costs;
        double m_update_factor;
        unsigned int m_max_updates_before_phase_I;

        Expr<Var, Var> m_objective_function;
        std::list<Var> m_artificial_variables;

        void save_objective_function(const Model& t_original_formulation);
        void create_artificial_variables(DantzigWolfe::Formulation& t_formulation);
        void find_initial_columns(idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration &t_column_generation);
        void delete_artificial_variables(DantzigWolfe::Formulation& t_formulation);
        bool all_artificial_variables_are_non_basic(const PrimalPoint &t_primal_values) const;
        void update_objective_function(DantzigWolfe::Formulation& t_formulation, const PrimalPoint& t_primal_values, bool t_include_original_objective_function);
        void restore_objective_function(DantzigWolfe::Formulation& t_formulation);
    public:
        Strategy(double t_initial_costs, double t_update_factor, unsigned int t_max_updates_before_phase_I);

        void execute(Optimizers::DantzigWolfeDecomposition &t_parent) override;
    };

    InfeasibilityStrategyFactory::Strategy *operator()() const override {
        return new Strategy(
                m_initial_costs.has_value() ? m_initial_costs.value() : 1e4,
                m_update_factor.has_value() ? m_update_factor.value() : 2.,
                m_max_updates_before_phase_I.has_value() ? m_max_updates_before_phase_I.value() : 4
        );
    }

    [[nodiscard]] ArtificialCosts *clone() const override {
        return new ArtificialCosts(*this);
    }

    ArtificialCosts& with_max_updates_before_phase_I(unsigned int t_value);

    ArtificialCosts& with_update_factor(double t_value);

    ArtificialCosts& with_initial_costs(double t_value);

};

#endif //IDOL_ARTIFICIALCOSTS_H
