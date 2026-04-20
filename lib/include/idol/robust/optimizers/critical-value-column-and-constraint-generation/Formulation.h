//
// Created by Henri on 17/04/2026.
//

#ifndef IDOL_CVCCG_FORMULATION_H
#define IDOL_CVCCG_FORMULATION_H
#include <vector>

#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::Optimizers::Robust {
    class CriticalValueColumnAndConstraintGeneration;
}

namespace idol::CVCCG {
    class Formulation;
}

class idol::CVCCG::Formulation {

    class Uncertainty {
        std::optional<Ctr> m_constraint;
    public:
        [[nodiscard]] bool is_constraint() const { return m_constraint.has_value(); }
        [[nodiscard]] const Ctr& ctr() const { return m_constraint.value(); }

        Uncertainty() = default;
        Uncertainty(const Ctr& t_ctr) : m_constraint(t_ctr) {}
    };

    const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& m_parent;
    bool m_all_linking_variables_are_binary = true;
    bool m_all_data_in_uncertainty_set_are_integer = true;
    bool m_has_linking_variables = false;

    Model m_master;
    Model m_sub_problem;
    std::vector<Uncertainty> m_uncertainties;

    void check_assumptions();
    void initialize_master();
    void initialize_sub_problem();
public:
    Formulation(const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& t_parent);

    Model& master() { return m_master; }
    Model& sub_problem() { return m_sub_problem; }

    void update_sub_problem_constraints(const PrimalPoint& t_master_solution);
    unsigned int n_uncertainties() const { return m_uncertainties.size(); }
    void update_sub_problem_objective(const PrimalPoint& t_master_solution, unsigned int t_index);
    void add_scenario(const PrimalPoint& t_scenario);
};

#endif //IDOL_CVCCG_FORMULATION_H