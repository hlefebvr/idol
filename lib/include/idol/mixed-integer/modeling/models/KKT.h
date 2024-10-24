//
// Created by henri on 30.08.24.
//

#ifndef IDOL_KKT_H
#define IDOL_KKT_H

#include "Model.h"
#include "idol/bilevel/modeling/LowerLevelDescription.h"

namespace idol::Reformulators {
    class KKT;
}

class idol::Reformulators::KKT {

    const Model& m_src_model;
    const Bilevel::LowerLevelDescription& m_description;

    std::vector<std::optional<Var>> m_dual_variables_for_constraints;
    std::vector<std::optional<Var>> m_dual_variables_for_lower_bounds;
    std::vector<std::optional<Var>> m_dual_variables_for_upper_bounds;
    std::vector<std::optional<Ctr>> m_dual_constraints;
    std::vector<std::optional<Ctr>> m_complementarity_constraints;
    Expr<Var, Var> m_dual_objective;

    void create_dual_variables_for_constraints();
    void create_dual_variables_for_bounds();
    void create_dual_constraints();
    void create_dual_constraint(const Var& t_var);
    void create_complementarity_constraints();
    void create_dual_objective();
public:
    KKT(const Model& t_src_model,
        const Bilevel::LowerLevelDescription& t_lower_level_description);

    void add_primal_variables(idol::Model &t_destination) const;
    void add_primal_constraints(Model &t_destination) const;
    void add_dual_variables(Model &t_destination) const;
    void add_dual_constraints(Model &t_destination) const;
    void add_complementarity_constraints(Model& t_destination) const;
    void add_strong_duality_constraint(Model& t_destination) const;
    void add_leader_objective(Model& t_destination) const;

    void add_kkt_reformulation(Model& t_destination) const;
    void add_strong_duality_reformulation(Model& t_destination) const;
    void add_dual(Model& t_destination) const;
};


#endif //IDOL_KKT_H
