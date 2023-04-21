#ifdef IDOL_USE_MOSEK

#include "optimizers/solvers/Optimizers_Mosek.h"
#include "linear-algebra/to_rotated_quadratic_cone.h"
#include "modeling/expressions/operations/operators.h"

#ifdef IDOL_USE_EIGEN
#include <Eigen/Sparse>
#endif

Optimizers::Mosek::Mosek(const Model &t_model, bool t_continuous_relaxation)
    : OptimizerWithLazyUpdates<MosekVar, MosekCtr>(t_model),
      m_model(new mosek::fusion::Model()),
      m_continuous_relaxation(t_continuous_relaxation) {

    m_model->setLogHandler(NULL);

}

Optimizers::Mosek::~Mosek() {
    m_model->dispose();
}

void Optimizers::Mosek::hook_build() {

    const auto& objective = parent().get_obj_expr();

    if (!objective.quadratic().empty()) {
        throw Exception("Handling quadratic objective is not implemented.");
    }

    hook_update_objective_sense();
    set_objective_as_updated();
    set_rhs_as_updated();
}

void Optimizers::Mosek::hook_optimize() {

    m_model->solve();

    auto problem_status = m_model->getProblemStatus();
    auto primal_status = m_model->getPrimalSolutionStatus();
    auto dual_status = m_model->getDualSolutionStatus();

    if (problem_status == mosek::fusion::ProblemStatus::PrimalAndDualFeasible
        && primal_status == mosek::fusion::SolutionStatus::Optimal
        && dual_status == mosek::fusion::SolutionStatus::Optimal) {

        m_solution_status = Optimal;
        m_solution_reason = Proved;

    } else if(problem_status == mosek::fusion::ProblemStatus::PrimalFeasible
              && primal_status == mosek::fusion::SolutionStatus::Optimal
              && dual_status == mosek::fusion::SolutionStatus::Undefined) {

        m_solution_status = Optimal;
        m_solution_reason = Proved;

    } else if (problem_status == mosek::fusion::ProblemStatus::PrimalInfeasible
               && primal_status == mosek::fusion::SolutionStatus::Undefined
               && dual_status == mosek::fusion::SolutionStatus::Certificate) {

        m_solution_status = Infeasible;
        m_solution_reason = Proved;

    } else if (problem_status == mosek::fusion::ProblemStatus::PrimalInfeasible
               && primal_status == mosek::fusion::SolutionStatus::Undefined
               && dual_status == mosek::fusion::SolutionStatus::Undefined
               ) {

        m_solution_status = Infeasible;
        m_solution_reason = Proved;

    } else if (problem_status == mosek::fusion::ProblemStatus::DualInfeasible
               && primal_status == mosek::fusion::SolutionStatus::Certificate
               && dual_status == mosek::fusion::SolutionStatus::Undefined) {

        m_solution_status = Unbounded;
        m_solution_reason = Proved;

    } else if (problem_status == mosek::fusion::ProblemStatus::Unknown
               && primal_status == mosek::fusion::SolutionStatus::Unknown
               && dual_status == mosek::fusion::SolutionStatus::Unknown) {

        m_solution_status = Fail;
        m_solution_reason = NotSpecified;

    } else if (problem_status == mosek::fusion::ProblemStatus::Unknown
               && primal_status == mosek::fusion::SolutionStatus::Unknown
               && dual_status == mosek::fusion::SolutionStatus::Undefined) {

        m_solution_status = Fail;
        m_solution_reason = NotSpecified;

    } else if (problem_status == mosek::fusion::ProblemStatus::PrimalInfeasibleOrUnbounded
               && primal_status == mosek::fusion::SolutionStatus::Unknown
               && dual_status == mosek::fusion::SolutionStatus::Undefined) {

        m_solution_status = InfOrUnbnd;
        m_solution_reason = NotSpecified;

    } else {
        std::cout << problem_status << ", " << primal_status << ", " << dual_status << std::endl;
        throw Exception("Unknown problem-solution status combination.");
    }
}

void Optimizers::Mosek::hook_write(const std::string &t_name) {
    m_model->writeTask(t_name);
}

MosekVar Optimizers::Mosek::hook_add(const Var &t_var, bool t_add_column) {

    MosekVar result;

    result.variable = m_model->variable(1, mosek::fusion::Domain::unbounded());

    const double lb = parent().get_var_lb(t_var);
    const double ub = parent().get_var_ub(t_var);
    const auto& column = parent().get_var_column(t_var);
    const auto type = parent().get_var_type(t_var);

    set_var_attr(result, type, lb, ub, as_numeric(column.obj()));

    if (t_add_column) {

        for (const auto& [ctr, constant] : column.linear()) {
            lazy(ctr).impl().constraint->index(0)->update(
                    mosek::fusion::Expr::mul(as_numeric(constant), result.variable->index(0)),
                    result.variable->index(0)
                );
        }

    }

    return result;

}

mosek::fusion::Expression::t Optimizers::Mosek::to_mosek_expression(const LinExpr<Var> &t_expr) const {

    auto result = mosek::fusion::Expr::constTerm(0);

    for (const auto& [var, constant] : t_expr) {
        result = mosek::fusion::Expr::add(
                std::move(result),
                mosek::fusion::Expr::mul(
                        as_numeric(constant),
                        lazy(var).impl().variable
                )
        );
    }

    return result;
}

mosek::fusion::Expression::t Optimizers::Mosek::to_mosek_expression(const Expr<Var> &t_expr) const {
    assert(t_expr.quadratic().empty());
    return mosek::fusion::Expr::add(
                as_numeric(t_expr.constant()),
                to_mosek_expression(t_expr.linear())
            );
}

MosekCtr Optimizers::Mosek::hook_add(const Ctr &t_ctr) {

    MosekCtr result;

    const auto& model = parent();

    const int type = model.get_ctr_type(t_ctr);
    const auto& row = model.get_ctr_row(t_ctr);

    if (!row.quadratic().empty()) {

#ifdef IDOL_USE_EIGEN

        auto sign = type == LessOrEqual ? 1. : -1.;

        auto rq_cone_expr = to_rotated_quadratic_cone(sign == 1. ? row.quadratic() : sign * row.quadratic());

        auto expression = mosek::fusion::Expr::zeros(0);

        auto it = rq_cone_expr.begin();

        if (!row.linear().empty()) {

            auto& head1 = *it;
            ++it;
            auto& head2 = *it;

            if (head1.is_zero() && head2.is_zero()) {

                expression = mosek::fusion::Expr::vstack(
                        std::move(expression),
                        mosek::fusion::Expr::constTerm(.5),
                        to_mosek_expression(sign * row.rhs() - sign * row.linear())
                );

            } else {
                throw Exception("Non-convex constraint was found.");
            }

            ++it;
        }

        for (const auto end = rq_cone_expr.end() ; it != end ; ++it) {
            expression = mosek::fusion::Expr::vstack(
                        std::move(expression),
                        to_mosek_expression(*it)
                    );
        }

        result.constraint = m_model->constraint(std::move(expression), mosek::fusion::Domain::inRotatedQCone());

        return result;
#else
      throw Exception("Modeling quadratic expressions with Mosek requires Eigen, please set the USE_EIGEN cmake option to YES to use this feature.");
#endif

    }

    // Build expression
    auto expr = to_mosek_expression(row.linear());
    expr = mosek::fusion::Expr::add(std::move(expr), -as_numeric(row.rhs()));

    // Set constraint type
    switch (type) {
        case LessOrEqual:
            result.constraint = m_model->constraint(std::move(expr), mosek::fusion::Domain::lessThan(0.));
        break;
        case GreaterOrEqual:
            result.constraint = m_model->constraint(std::move(expr), mosek::fusion::Domain::greaterThan(0.));
            break;
        case Equal:
            result.constraint = m_model->constraint(std::move(expr), mosek::fusion::Domain::equalsTo(0.));
            break;
        default: throw Exception("Enum out of bounds.");
    }

    return result;
}

void Optimizers::Mosek::hook_update_objective_sense() {
    //std::cout << "skips updating sense" << std::endl;
}

void Optimizers::Mosek::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {
    throw Exception("Not implemented hook_update_matrix");
}

void Optimizers::Mosek::hook_update() {

}

void Optimizers::Mosek::hook_update(const Var &t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const Constant& obj = model.get_var_column(t_var).obj();

    set_var_attr(impl, type, lb, ub, as_numeric(obj));

}

void Optimizers::Mosek::hook_update(const Ctr &t_ctr) {

    const auto& row = parent().get_ctr_row(t_ctr);

    auto rhs = std::make_shared<monty::ndarray< double >>(monty::shape(1), -as_numeric(row.rhs()));
    lazy(t_ctr).impl().constraint->index(0)->update(rhs);

}

void Optimizers::Mosek::hook_update_objective() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();
    const int sense = model.get_obj_sense();

    auto expr = mosek::fusion::Expr::constTerm(as_numeric(objective.constant()));

    for (const auto& [var, constant] : objective.linear()) {
        expr = mosek::fusion::Expr::add(
                std::move(expr),
                mosek::fusion::Expr::mul(
                        as_numeric(constant),
                        lazy(var).impl().variable
                )
        );
    }

    if (sense == Minimize) {
        m_model->objective(mosek::fusion::ObjectiveSense::Minimize, std::move(expr));
    } else {
        m_model->objective(mosek::fusion::ObjectiveSense::Maximize, std::move(expr));
    }

}

void Optimizers::Mosek::hook_update_rhs() {
    throw Exception("Not implemented hook_update_rhs");
}

void Optimizers::Mosek::hook_remove(const Var &t_var) {

    auto& impl = lazy(t_var).impl();

    m_model->updateObjective(mosek::fusion::Expr::constTerm(0), impl.variable);

    impl.variable->remove();
    impl.variable = nullptr;

    if (impl.lower_bound.get()) {
        impl.lower_bound->remove();
        impl.lower_bound = nullptr;
    }

    if (impl.upper_bound.get()) {
        impl.upper_bound->remove();
        impl.upper_bound = nullptr;
    }
}

void Optimizers::Mosek::hook_remove(const Ctr &t_ctr) {
    auto& impl = lazy(t_ctr).impl();
    impl.constraint->remove();
    impl.constraint = nullptr;
}

void Optimizers::Mosek::set_var_attr(MosekVar &t_mosek_var, int t_type, double t_lb, double t_ub, double t_obj) {

    // Set obj
    m_model->updateObjective(mosek::fusion::Expr::mul(t_obj, t_mosek_var.variable), t_mosek_var.variable);

    // Set type
    if (m_continuous_relaxation || t_type == Continuous) {
        t_mosek_var.variable->make_continuous();
    } else {
        t_mosek_var.variable->make_integer();
    }

    // Set bounds
    set_var_lb(t_mosek_var, t_lb);
    set_var_ub(t_mosek_var, t_ub);
}

void Optimizers::Mosek::set_var_lb(MosekVar &t_mosek_var, double t_bound) {

    const bool has_lb = !is_neg_inf(t_bound);

    if (!has_lb) {
        if (t_mosek_var.lower_bound.get()) {
            t_mosek_var.lower_bound->remove();
            t_mosek_var.lower_bound = nullptr;
        }
        return;
    }

    if (t_mosek_var.lower_bound.get()) {
        t_mosek_var.lower_bound->update(mosek::fusion::Expr::sub(t_mosek_var.variable, t_bound));
        return;
    }

    t_mosek_var.lower_bound = m_model->constraint(mosek::fusion::Expr::sub(t_mosek_var.variable, t_bound), mosek::fusion::Domain::greaterThan(0.));

}

void Optimizers::Mosek::set_var_ub(MosekVar &t_mosek_var, double t_bound) {

    const bool has_ub = !is_pos_inf(t_bound);

    if (!has_ub) {
        if (t_mosek_var.upper_bound.get()) {
            t_mosek_var.upper_bound->remove();
            t_mosek_var.upper_bound = nullptr;
        }
        return;
    }

    if (t_mosek_var.upper_bound.get()) {
        t_mosek_var.upper_bound->update(mosek::fusion::Expr::sub(t_mosek_var.variable, t_bound));
        return;
    }

    t_mosek_var.upper_bound = m_model->constraint(mosek::fusion::Expr::sub(t_mosek_var.variable, t_bound), mosek::fusion::Domain::lessThan(0.));

}

void Optimizers::Mosek::set_time_limit(double t_time_limit) {
    m_model->setSolverParam("optimizerMaxTime", t_time_limit);
    Optimizer::set_time_limit(t_time_limit);
}

void Optimizers::Mosek::set_thread_limit(unsigned int t_thread_limit) {
    m_model->setSolverParam("numThreads", (int) t_thread_limit);
    Optimizer::set_thread_limit(t_thread_limit);
}

void Optimizers::Mosek::set_best_obj_stop(double t_best_obj_stop) {
    m_model->setSolverParam("upperObjCut", t_best_obj_stop);
    Optimizer::set_best_obj_stop(t_best_obj_stop);
}

void Optimizers::Mosek::set_best_bound_stop(double t_best_bound_stop) {
    m_model->setSolverParam("lowerObjCut", t_best_bound_stop);
    Optimizer::set_best_bound_stop(t_best_bound_stop);
}

void Optimizers::Mosek::set_presolve(bool t_value) {
    m_model->setSolverParam("presolveUse", t_value ? "on" : "off");
    Optimizer::set_presolve(t_value);
}

void Optimizers::Mosek::set_infeasible_or_unbounded_info(bool t_value) {
    m_model->acceptedSolutionStatus(t_value ? mosek::fusion::AccSolutionStatus::Anything : mosek::fusion::AccSolutionStatus::Feasible);
    Optimizer::set_infeasible_or_unbounded_info(t_value);
}

SolutionStatus Optimizers::Mosek::get_status() const {
    return m_solution_status;
}

SolutionReason Optimizers::Mosek::get_reason() const {
    return m_solution_reason;
}

double Optimizers::Mosek::get_best_obj() const {
    if (m_solution_status == Infeasible) { return Inf; }
    if (m_solution_status == Unbounded) { return -Inf; }
    return m_model->primalObjValue();
}

double Optimizers::Mosek::get_best_bound() const {
    if (m_solution_status == Infeasible) { return Inf; }
    if (m_solution_status == Unbounded) { return -Inf; }
    return m_model->dualObjValue();
}

double Optimizers::Mosek::get_var_primal(const Var &t_var) const {
    return lazy(t_var).impl().variable->level()->operator[](0);
}

double Optimizers::Mosek::get_var_ray(const Var &t_var) const {
    if (m_model->getPrimalSolutionStatus() != mosek::fusion::SolutionStatus::Certificate) {
        throw Exception("Ray not available.");
    }
    return lazy(t_var).impl().variable->level()->operator[](0);
}

double Optimizers::Mosek::get_ctr_dual(const Ctr &t_ctr) const {
    return lazy(t_ctr).impl().constraint->dual()->operator[](0);
}

double Optimizers::Mosek::get_ctr_farkas(const Ctr &t_ctr) const {
    if (m_model->getDualSolutionStatus() != mosek::fusion::SolutionStatus::Certificate) {
        throw Exception("Farkas certificate not available.");
    }
    return lazy(t_ctr).impl().constraint->dual()->operator[](0);
}

double Optimizers::Mosek::get_relative_gap() const {
    throw Exception("Not implemented.");
}

double Optimizers::Mosek::get_absolute_gap() const {
    throw Exception("Not implemented.");
}

unsigned int Optimizers::Mosek::get_n_solutions() const {
    const auto status = get_status();
    return status == Optimal || status == Feasible;
}

unsigned int Optimizers::Mosek::get_solution_index() const {
    return 0;
}

void Optimizers::Mosek::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds.");
    }
}

MosekKiller::~MosekKiller() {
    mosek::releaseGlobalEnv();
}

#endif
