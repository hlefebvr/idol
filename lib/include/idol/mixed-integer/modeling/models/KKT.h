//
// Created by henri on 21.11.24.
//

#ifndef IDOL_KKT_H
#define IDOL_KKT_H

#include <vector>
#include <variant>
#include <cassert>
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/constraints/QCtr.h"
#include "idol/mixed-integer/modeling/expressions/QuadExpr.h"

namespace idol {
    namespace Reformulators {
        class KKT;
    }
    class Model;

    namespace Bilevel {
        class Description;
    }
}

class idol::Reformulators::KKT {
    const Model& m_primal;
    const QuadExpr<Var, double>& m_primal_objective;
    std::string m_prefix;

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
    class BoundProvider;

    KKT(const Model& t_parent,
              const QuadExpr<Var, double>& t_primal_objective,
              const std::function<bool(const Var&)>& t_primal_variable_indicator = [](const Var&) { return true; },
              const std::function<bool(const Ctr&)>& t_primal_constraint_indicator = [](const Ctr&) { return true; },
              const std::function<bool(const QCtr&)>& t_primal_qconstraint_indicator = [](const QCtr&) { return true; });

    explicit KKT(const Model& t_parent,
                      const std::function<bool(const Var&)>& t_primal_variable_indicator = [](const Var&) { return true; },
                      const std::function<bool(const Ctr&)>& t_primal_constraint_indicator = [](const Ctr&) { return true; },
                      const std::function<bool(const QCtr&)>& t_primal_qconstraint_indicator = [](const QCtr&) { return true; });

    KKT(const Model& t_high_point_relaxation, const Bilevel::Description& t_bilevel_description);

    void set_prefix(std::string t_prefix) { m_prefix = std::move(t_prefix); }

    [[nodiscard]] const QuadExpr<Var, double>& get_dual_obj_expr() const { return m_dual_objective; }

    /**
     * Adds the coupling variables and constraints to the destination model.
     * @param t_destination
     */
    void add_coupling(Model& t_destination);
    void add_coupling_variables(Model& t_destination);
    void add_coupling_constraints(Model& t_destination);

    /**
     * Adds the dual of the primal problem to the destination model.
     * The dual is considered only for the indicated constraints and variables.
     * @param t_destination
     * @param t_add_objective
     */
    void add_dual(Model& t_destination, bool t_add_objective = true);
    void add_dual_variables(Model& t_destination);
    void add_dual_constraints(Model& t_destination);
    void add_dual_objective(Model& t_destination);

    /**
     * Adds the primal of the primal problem to the destination model.
     * The primal is considered only for the indicated constraints and variables.
     * @param t_destination
     */
    void add_primal(Model& t_destination, bool t_add_objective = true);
    void add_primal_variables(idol::Model &t_destination);
    void add_primal_constraints(idol::Model &t_destination);
    void add_primal_objective(idol::Model &t_destination);

    /**
     * Adds the strong duality reformulation to the destination model.
     * The strong duality reformulation is considered only for the indicated constraints and variables.
     * @param t_destination
     */
    void add_strong_duality_reformulation(Model& t_destination);

    /**
     * Adds the KKT reformulation to the destination model.
     * The KKT reformulation is considered only for the indicated constraints and variables.
     * @param t_destination
     */
    void add_kkt_reformulation(idol::Model &t_destination, bool t_use_sos1 = false);

    void add_kkt_reformulation(idol::Model &t_destination, BoundProvider& t_bound_provider);

    void add_bounds_on_dual_variables(idol::Model &t_destination, BoundProvider& t_bound_provider);

    [[nodiscard]] const Var& get_dual_var(const Ctr& t_ctr) const;
    [[nodiscard]] const Var& get_dual_var_lb(const Var& t_var) const;
    [[nodiscard]] const Var& get_dual_var_ub(const Var& t_var) const;
    const std::variant<Ctr, QCtr>& get_dual_ctr(const Var& t_var) const;
};

class idol::Reformulators::KKT::BoundProvider {
    const Model* m_model = nullptr;
public:
    virtual ~BoundProvider() = default;

    [[nodiscard]] const Model& model() const { return *m_model; }

    virtual double get_ctr_dual_lb(const Ctr& t_ctr) = 0;
    virtual double get_ctr_dual_ub(const Ctr& t_ctr) = 0;
    virtual double get_ctr_slack_lb(const Ctr& t_ctr) = 0;
    virtual double get_ctr_slack_ub(const Ctr& t_ctr) = 0;
    virtual double get_var_lb_dual_ub(const Var& t_var) = 0;
    virtual double get_var_ub_dual_lb(const Var& t_var) = 0;
    virtual double get_var_lb(const Var& t_var) { assert(m_model); return m_model->get_var_lb(t_var); }
    virtual double get_var_ub(const Var& t_var) { assert(m_model); return m_model->get_var_ub(t_var); }

    [[nodiscard]] virtual BoundProvider* clone() const = 0;

    friend class idol::Reformulators::KKT;
};

#endif //IDOL_KKT_H
