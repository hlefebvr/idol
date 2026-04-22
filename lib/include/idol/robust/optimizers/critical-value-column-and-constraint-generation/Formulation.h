//
// Created by Henri on 17/04/2026.
//

#ifndef IDOL_CVCCG_FORMULATION_H
#define IDOL_CVCCG_FORMULATION_H
#include <utility>
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

    struct GeneratedScenario {
        PrimalPoint scenario;
        PrimalPoint master_solution;
    };

    struct LinkingConstraint {
        Ctr ctr_in_uncertainty_set;
        Map<long int, Var> critical_values;

        LinkingConstraint(Ctr  t_ctr) : ctr_in_uncertainty_set(std::move(t_ctr)) {}
    };

    const Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& m_parent;

    // Analysis of the model
    std::list<Var> m_linking_variables;
    std::list<LinkingConstraint> m_linking_constraints;
    std::vector<Uncertainty> m_uncertainties;
    bool m_all_linking_variables_are_binary = true;
    bool m_all_data_in_linking_constraints_is_integer = true;
    bool m_use_indicator = false;

    Model m_master;
    Model m_sub_problem;

    // Scenarios
    std::list<GeneratedScenario> m_scenario_pool;
    std::list<std::list<GeneratedScenario>::iterator> m_currently_present_scenarios;

    void check_assumptions();
    void initialize_master();
    void initialize_sub_problem();

    void add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool, Uncertainty& t_uncertainty);
public:
    Formulation(const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& t_parent);

    Model& master() { return m_master; }
    Model& sub_problem() { return m_sub_problem; }

    void update_sub_problem_constraints(const PrimalPoint& t_master_solution);
    unsigned int n_uncertainties() const { return m_uncertainties.size(); }
    void update_sub_problem_objective(const PrimalPoint& t_master_solution, unsigned int t_index);
    std::list<GeneratedScenario>::iterator add_scenario_to_pool(PrimalPoint&& t_scenario, PrimalPoint&& t_master_scenario);
    void add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool);
};

#endif //IDOL_CVCCG_FORMULATION_H