//
// Created by henri on 30.08.24.
//

#ifndef IDOL_KKT_H
#define IDOL_KKT_H

#include "idol/modeling/models/Model.h"

namespace idol::Reformulators {
    class KKT;
}

class idol::Reformulators::KKT {

    const Model& m_src_model;
    std::function<bool(const Var&)> m_indicator_for_vars;
    std::function<bool(const Ctr&)> m_indicator_for_ctrs;
    const bool m_keep_original_variables = true; // If false, original variables are seen as constants in the KKT model.
    const bool m_keep_primal_objects = true; // If false, new objects (variables and constraints) are created for the KKT model.

    std::vector<std::optional<Var>> m_primal_vars;
    std::vector<std::optional<Ctr>> m_primal_ctrs;

    std::vector<std::optional<Var>> m_dual_vars_for_constraints;
    std::vector<std::optional<Var>> m_dual_vars_for_upper_bounds;
    std::vector<std::optional<Var>> m_dual_vars_for_lower_bounds;
    std::vector<std::optional<Ctr>> m_dual_ctrs;

    const Var& to_destination_space(const Var& t_src_var) const; // Returns the primal variable used to represent the given primal variable in the source model.
    Row to_destination_space(const Row& t_src_row) const;

    const Var& dual(const Ctr& t_ctr) const; // Returns the dual variable associated with the given constraint.

    void create_primal_variables();
    void create_primal_constraints();

    void create_dual_variables_for_constraints();

    static void add_variables(Model& t_destination, const std::vector<std::optional<Var>>& t_vars);
    static void add_constraints(Model& t_destination, const std::vector<std::optional<Ctr>>& t_ctrs);
public:
    KKT(const Model& t_src_model,
        const std::function<bool(const Var&)>& t_indicator_for_vars,
        const std::function<bool(const Ctr&)>& t_indicator_for_ctrs);

    void add_primal_variables(Model& t_destination);
    void add_primal_constraints(Model& t_destination);

    void add_dual_variables(Model& t_destination);
    void add_dual_constraints(Model& t_destination);
};


#endif //IDOL_KKT_H
