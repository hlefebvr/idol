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
        double penalty;
    };

    class Uncertainty {
        std::optional<Ctr> m_constraint_in_original_model;
        std::list<CurrentlyPresentCut> m_currently_present_cuts;
        std::list<CurrentlyPresentCut> m_not_currently_present_cuts;
    public:
        [[nodiscard]] bool is_constraint() const { return m_constraint_in_original_model.has_value(); }
        [[nodiscard]] const Ctr& ctr() const { return m_constraint_in_original_model.value(); }
        [[nodiscard]] auto currently_present_cuts() const { return ConstIteratorForward(m_currently_present_cuts); }
        auto currently_present_cuts() { return IteratorForward(m_currently_present_cuts); }
        void add_currently_present_cut(const Ctr& t_ctr, std::list<GeneratedScenario>::iterator t_scenario, double t_penalty) { m_currently_present_cuts.emplace_back(t_ctr, t_scenario, t_penalty); }
        auto remove_from_currently_present_cuts(const std::list<CurrentlyPresentCut>::const_iterator& t_it) { return m_currently_present_cuts.erase(t_it); }

        Uncertainty() = default;
        Uncertainty(const Ctr& t_ctr) : m_constraint_in_original_model(t_ctr) {}
    };

    const Optimizers::Robust::CriticalValueColumnAndConstraintGeneration& m_parent;

    // Analysis of the model
    std::list<Var> m_linking_variables;
    bool m_master_is_continuous = true;
    bool m_all_linking_variables_are_binary = true;
    bool m_all_data_in_linking_constraints_is_integer = true;
    const bool m_use_cover_constraints = true;

    Model m_master;
    Model m_sub_problem;
    std::optional<Var> m_epigraph_variable;

    // Scenarios
    std::list<GeneratedScenario> m_scenario_pool;
    std::list<LinkingConstraint> m_linking_constraints;
    std::list<Uncertainty> m_uncertainties;
    unsigned int m_n_critical_values = 0;

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

    void update_sub_problem_rhs(const PrimalPoint& t_master_solution);
    unsigned int n_uncertainties() const { return m_uncertainties.size(); }
    void update_sub_problem_objective(const PrimalPoint& t_master_solution, const Uncertainty& t_uncertainty);
    std::list<GeneratedScenario>::iterator add_scenario_to_pool(PrimalPoint&& t_scenario, PrimalPoint&& t_master_scenario);
    void add_scenario_to_master(const std::list<GeneratedScenario>::iterator& t_iterator_in_pool);

    unsigned int n_critical_values() const { return m_n_critical_values; }
    unsigned int n_scenarios_in_pool() const { return m_scenario_pool.size(); }
    double get_scenario_var_primal(const Var& t_var) const;

    bool master_provides_a_valid_bound() const;
    const Var& epigraph_variable() const { return *m_epigraph_variable; }

    void remove_cut_if(Uncertainty& t_uncertainty, const std::function<bool(const Ctr&, const PrimalPoint&)>& t_indicator);
    void set_unc_var_lb(const Var& t_var, double t_lb);
    void set_unc_var_ub(const Var& t_var, double t_ub);
    void load_column_from_pool();
};

#endif //IDOL_CVCCG_FORMULATION_H