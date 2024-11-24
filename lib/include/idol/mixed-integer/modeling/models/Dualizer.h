//
// Created by henri on 21.11.24.
//

#ifndef IDOL_DUALIZER_H
#define IDOL_DUALIZER_H

#include <vector>
#include <variant>
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/constraints/QCtr.h"
#include "idol/mixed-integer/modeling/expressions/QuadExpr.h"

namespace idol {
    class Dualizer;
    class Model;
}

class idol::Dualizer {
    const Model& m_primal;
    const QuadExpr<Var, double>& m_primal_objective;

    std::function<bool(const Var&)> m_primal_variable_indicator;
    std::function<bool(const Ctr&)> m_primal_constraint_indicator;
    std::function<bool(const QCtr&)> m_primal_qconstraint_indicator;

    QuadExpr<Var, double> m_dual_objective;
    std::vector<std::optional<Var>> m_dual_variables_for_constraints; // constraint index <-> dual variable
    std::vector<std::optional<Var>> m_dual_variables_for_qconstraints; // qconstraint index <-> dual variable
    std::vector<std::optional<Var>> m_dual_variables_for_lower_bounds; // variable index <-> dual variable
    std::vector<std::optional<Var>> m_dual_variables_for_upper_bounds; // variable index <-> dual variable
    std::vector<std::optional<std::variant<Ctr, QCtr>>> m_dual_constraints; // variable index <-> dual constraint

    [[nodiscard]] bool primal_is_a_linear_problem() const;
    [[nodiscard]] bool is_linear(const QuadExpr<Var, double>& t_expr) const;
    void create_dual_variables();
    [[nodiscard]] std::pair<double, double> bounds_for_dual_variable(idol::CtrType t_type) const;
    void create_dual_constraints();
public:
    explicit Dualizer(const Model& t_parent,
                      const QuadExpr<Var, double>& t_primal_objective,
                      const std::function<bool(const Var&)>& t_primal_variable_indicator = [](const Var&) { return true; },
                      const std::function<bool(const Ctr&)>& t_primal_constraint_indicator = [](const Ctr&) { return true; },
                      const std::function<bool(const QCtr&)>& t_primal_qconstraint_indicator = [](const QCtr&) { return true; });

    const QuadExpr<Var, double>& get_dual_obj_expr() const { return m_dual_objective; }

    void add_coupling(Model& t_destination);
    void add_coupling_variables(Model& t_destination);
    void add_coupling_constraints(Model& t_destination);

    void add_dual(Model& t_destination);
    void add_dual_variables(Model& t_destination);
    void add_dual_constraints(Model& t_destination);
    void add_dual_objective(Model& t_destination);

    void add_primal(Model& t_destination);
    void add_primal_variables(idol::Model &t_destination);
    void add_primal_constraints(idol::Model &t_destination);
    void add_primal_objective(idol::Model &t_destination);

    void add_strong_duality_reformulation(Model& t_destination);
    void add_kkt_reformulation(idol::Model &t_destination);
};

#endif //IDOL_DUALIZER_H
