//
// Created by henri on 11.12.24.
//

#ifndef IDOL_CCG_FORMULATION_H
#define IDOL_CCG_FORMULATION_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/robust/modeling/Description.h"

namespace idol::CCG {
    class Formulation;
}

class idol::CCG::Formulation {
    const Model& m_parent;
    const ::idol::Robust::Description &m_description;

    Model m_master;
    std::vector<Var> m_second_stage_variables;
    std::vector<Ctr> m_second_stage_constraints;

    unsigned int m_n_added_scenario = 0;
    std::optional<Var> m_second_stage_epigraph;

    void parse_variables();
    void parse_objective();
    void parse_constraints();
public:
    Formulation(const Model& t_parent, const ::idol::Robust::Description &t_description);

    Model& master() { return m_master; }

    const Model& master() const { return m_master; }

    void add_scenario_to_master(const Point<Var>& t_scenario);
};

#endif //IDOL_CCG_FORMULATION_H
