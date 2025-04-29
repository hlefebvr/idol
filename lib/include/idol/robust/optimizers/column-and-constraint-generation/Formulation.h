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
    ::idol::Bilevel::Description m_bilevel_description_separation;

    Model m_master;
    std::optional<::idol::Bilevel::Description> m_bilevel_description_master;
    std::vector<Var> m_second_stage_variables;
    std::vector<Ctr> m_second_stage_constraints;
    std::vector<Ctr> m_linking_constraints;
    std::vector<Ctr> m_coupling_constraints;

    unsigned int m_n_added_scenario = 0;
    bool m_has_second_stage_objective = false;
    std::optional<Var> m_second_stage_epigraph;

    void parse_variables();
    void parse_objective();
    void parse_constraints();
    void copy_bilevel_description(const ::idol::Bilevel::Description& t_src, const ::idol::Bilevel::Description& t_dest) const;
    void add_separation_problem_constraints(idol::Model &t_model, const idol::Point<idol::Var> &t_first_stage_decision);
    QuadExpr<Var> compute_second_stage_objective(const Point<Var>& t_first_stage_decision) const;
public:
    Formulation(const Model& t_parent,
                const ::idol::Robust::Description &t_robust_description,
                const ::idol::Bilevel::Description &t_bilevel_description
                );

    Model& master() { return m_master; }

    const Model& master() const { return m_master; }

    void add_scenario_to_master(const Point<Var>& t_scenario, bool t_add_annotation);

    Model build_optimality_separation_problem_for_adjustable_robust_problem(const Point<Var>& t_first_stage_decision, unsigned int t_coupling_constraint_index);

    std::pair<Model, std::vector<Var>> build_feasibility_separation_problem(const Point<Var>& t_first_stage_decision);

    std::pair<Model, std::vector<Var>> build_joint_separation_problem(const Point<Var>& t_first_stage_decision);

    unsigned int n_coupling_constraints() const { return 1 + m_coupling_constraints.size(); }

    unsigned int n_added_scenarios() const { return m_n_added_scenario; }

    const Bilevel::Description& bilevel_description_separation() const { return m_bilevel_description_separation; }

    const Bilevel::Description& bilevel_description_master() const { return *m_bilevel_description_master; }

    bool is_adjustable_robust_problem() const;

    bool is_wait_and_see_follower() const { return !is_adjustable_robust_problem(); }

    bool should_have_epigraph_and_epigraph_is_not_in_master() const;

    const Var& second_stage_epigraph() const { return *m_second_stage_epigraph; }
};

#endif //IDOL_CCG_FORMULATION_H
