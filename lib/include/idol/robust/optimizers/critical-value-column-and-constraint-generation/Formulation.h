//
// Created by Henri on 17/04/2026.
//

#ifndef IDOL_CVCCG_FORMULATION_H
#define IDOL_CVCCG_FORMULATION_H
#include <utility>
#include <vector>

#include "idol/general/utils/Optional.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::Optimizers::Robust {
    class CriticalValueColumnAndConstraintGeneration;
}

namespace idol::CVCCG {
    class Formulation;
}

class idol::CVCCG::Formulation {

    struct LinkingConstraint {
        Ctr ctr_in_uncertainty_set;
        Map<long int, std::pair<Var, Ctr>> critical_values;
        Optional<Ctr> cover_constraint;

        LinkingConstraint(Ctr  t_ctr) : ctr_in_uncertainty_set(std::move(t_ctr)) {}
    };

    struct GeneratedScenario {
        PrimalPoint scenario;
        PrimalPoint master_solution;
    };

    struct CurrentlyPresentCut {
        Ctr cut;
        std::list<GeneratedScenario>::iterator scenario;
        std::list<std::pair<std::list<LinkingConstraint>::iterator, long int>> linking_constraints;
    };

    class Uncertainty {
        std::optional<Ctr> m_constraint_in_original_model;
        std::list<CurrentlyPresentCut> m_currently_present_cuts;
    public:
        [[nodiscard]] bool is_constraint() const { return m_constraint_in_original_model.has_value(); }
        [[nodiscard]] const Ctr& ctr() const { return m_constraint_in_original_model.value(); }
        [[nodiscard]] auto currently_present_cuts() const { return ConstIteratorForward(m_currently_present_cuts); }
        auto currently_present_cuts() { return IteratorForward(m_currently_present_cuts); }

        Uncertainty() = default;
        Uncertainty(const Ctr& t_ctr) : m_constraint_in_original_model(t_ctr) {}
    };

    const Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& m_parent;

    // Analysis of the model
    std::list<Var> m_linking_variables;
    bool m_all_linking_variables_are_binary = true;
    bool m_all_data_in_linking_constraints_is_integer = true;
    const bool m_use_indicator = false;
    const bool m_use_cover_constraints = true;

    Model m_master;
    Model m_sub_problem;

    // Scenarios
    std::list<GeneratedScenario> m_scenario_pool;
    std::list<LinkingConstraint> m_linking_constraints;
    std::list<Uncertainty> m_uncertainties;

    void check_assumptions();
    void initialize_master();
    void initialize_sub_problem();

    void add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool, Uncertainty& t_uncertainty);
    void create_critical_value_variable_if_needed(const PrimalPoint& t_scenario);
    void create_critical_value_variable(const PrimalPoint& t_scenario, LinkingConstraint& t_linking);
    double compute_critical_value(const Ctr& t_ctr, const PrimalPoint& t_scenario) const;
public:
    Formulation(const idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& t_parent);

    Model& master() { return m_master; }
    Model& sub_problem() { return m_sub_problem; }

    auto uncertainties() { return IteratorForward(m_uncertainties); }
    auto uncertainties() const { return ConstIteratorForward(m_uncertainties); }

    void update_sub_problem_constraints(const PrimalPoint& t_master_solution);
    unsigned int n_uncertainties() const { return m_uncertainties.size(); }
    void update_sub_problem_objective(const PrimalPoint& t_master_solution, const Uncertainty& t_uncertainty);
    std::list<GeneratedScenario>::iterator add_scenario_to_pool(PrimalPoint&& t_scenario, PrimalPoint&& t_master_scenario);
    void add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool);
};

#endif //IDOL_CVCCG_FORMULATION_H