//
// Created by henri on 11.12.24.
//

#ifndef IDOL_CCG_FORMULATION_H
#define IDOL_CCG_FORMULATION_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::CCG {
    class Formulation;
}

class idol::CCG::Formulation {
    const Model& m_parent;
    const ::idol::Robust::Description &m_robust_description;
    const ::idol::Bilevel::Description &m_bilevel_description;
    ::idol::Bilevel::Description m_separation_bilevel_description;

    Model m_master;
    std::vector<Var> m_second_stage_variables;
    std::vector<Ctr> m_second_stage_constraints;
    std::vector<Ctr> m_linking_constraints;
    std::vector<Ctr> m_coupling_constraints; // TODO: so far, these are not handled, i.e., detected

    unsigned int m_n_added_scenario = 0;
    std::optional<Var> m_second_stage_epigraph;

    void parse_variables();
    void parse_objective();
    void parse_constraints();
    void copy_bilevel_description();
public:
    Formulation(const Model& t_parent,
                const ::idol::Robust::Description &t_robust_description,
                const ::idol::Bilevel::Description &t_bilevel_description
                );

    Model& master() { return m_master; }

    const Model& master() const { return m_master; }

    void add_scenario_to_master(const Point<Var>& t_scenario);

    Model build_optimality_separation_problem_for_adjustable_robust_problem(const Point<Var>& t_first_stage_decision, unsigned int t_coupling_constraint_index);

    unsigned int n_coupling_constraints() const { return 1 + m_coupling_constraints.size(); }

    unsigned int n_added_scenarios() const { return m_n_added_scenario; }

    const Bilevel::Description& separation_bilevel_description() const { return m_separation_bilevel_description; }
};

#endif //IDOL_CCG_FORMULATION_H
