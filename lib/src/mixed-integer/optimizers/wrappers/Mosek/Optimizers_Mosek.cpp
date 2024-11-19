#ifdef IDOL_USE_MOSEK

#include "idol/mixed-integer/optimizers/wrappers/Mosek/Optimizers_Mosek.h"
#include "idol/general/linear-algebra/to_rotated_quadratic_cone.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

#ifdef IDOL_USE_EIGEN
#include <Eigen/Sparse>
#endif

idol::Optimizers::Mosek::Mosek(const Model &t_model, bool t_continuous_relaxation)
    : OptimizerWithLazyUpdates(t_model),
      m_model(new mosek::fusion::Model()),
      m_continuous_relaxation(t_continuous_relaxation) {

    m_model->acceptedSolutionStatus(mosek::fusion::AccSolutionStatus::Feasible);

    if (!get_param_logs()) {
        m_model->setLogHandler(NULL);
    }

}

idol::Optimizers::Mosek::~Mosek() {
    m_model->dispose();
}

void idol::Optimizers::Mosek::hook_build() {

    const auto& objective = parent().get_obj_expr();

    set_rhs_as_updated();
}

void idol::Optimizers::Mosek::hook_optimize() {

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

    } else if (problem_status == mosek::fusion::ProblemStatus::PrimalFeasible
               && primal_status == mosek::fusion::SolutionStatus::Feasible
               && dual_status == mosek::fusion::SolutionStatus::Undefined) {

        m_solution_status = Feasible;
        if (time().count() >= get_param_time_limit()) {
            m_solution_reason = TimeLimit;
        } else {
            m_solution_reason = NotSpecified;
        }

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
        if (time().count() >= get_param_time_limit()) {
            m_solution_reason = TimeLimit;
        } else {
            m_solution_reason = NotSpecified;
        }

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

void idol::Optimizers::Mosek::hook_write(const std::string &t_name) {
    m_model->writeTask(t_name);
}

idol::MosekVar idol::Optimizers::Mosek::hook_add(const Var &t_var, bool t_add_column) {

    MosekVar result;

    result.variable = m_model->variable(/* t_var.name(), */ 1, mosek::fusion::Domain::unbounded());

    const double lb = parent().get_var_lb(t_var);
    const double ub = parent().get_var_ub(t_var);
    const double obj = parent().get_var_obj(t_var);
    const auto& column = parent().get_var_column(t_var);
    const auto type = parent().get_var_type(t_var);

    set_var_attr(result, type, lb, ub, obj);

    if (t_add_column) {

        for (const auto& [ctr, constant] : column) {
            lazy(ctr).impl().constraint->index(0)->update(
                    mosek::fusion::Expr::mul(constant, result.variable->index(0)),
                    result.variable->index(0)
                );
        }

    }

    return result;

}

mosek::fusion::Expression::t idol::Optimizers::Mosek::to_mosek_expression(const LinExpr<Var> &t_expr) const {

    auto result = mosek::fusion::Expr::constTerm(0);

    for (const auto& [var, constant] : t_expr) {
        result = mosek::fusion::Expr::add(
                std::move(result),
                mosek::fusion::Expr::mul(
                        constant,
                        lazy(var).impl().variable
                )
        );
    }

    return result;
}

mosek::fusion::Expression::t idol::Optimizers::Mosek::to_mosek_expression(const AffExpr<Var> &t_expr) const {
    return mosek::fusion::Expr::add(
                t_expr.constant(),
                to_mosek_expression(t_expr.linear())
            );
}

idol::MosekCtr idol::Optimizers::Mosek::hook_add(const Ctr &t_ctr) {

    MosekCtr result;

    const auto& model = parent();

    const int type = model.get_ctr_type(t_ctr);
    const auto& row = model.get_ctr_row(t_ctr);
    const double rhs = model.get_ctr_rhs(t_ctr);

    /*
    if (!row.quadratic().empty()) {

#ifdef IDOL_USE_EIGEN

        auto sign = type == LessOrEqual ? 1. : -1.;

        auto rq_cone_expr = to_rotated_quadratic_cone(sign == 1. ? row.quadratic() : sign * row.quadratic());

        auto expression = mosek::fusion::AffExpr::zeros(0);

        auto it = rq_cone_expr.begin();

        if (!row.linear().empty() || std::abs(rhs) >= Tolerance::Sparsity) {

            const auto& head1 = *it;
            ++it;
            const auto& head2 = *it;
            ++it;

            // Here, we have a constraint of the form c^T x + x^T Q x <= b \iff 2 * (1/2) * (b - c^Tx) >= (x^T Q x) = [Q^{1/2} x]^2
            if (head1.is_zero() && head2.is_zero()) {

                expression = mosek::fusion::AffExpr::vstack(
                        std::move(expression),
                        mosek::fusion::AffExpr::constTerm(.5),
                        to_mosek_expression(sign * rhs - sign * row.linear())
                );

            } else if (row.linear().empty()) { // Here, we have a constraint of the form x^T Q x - 2yz <= b \iff 2 * .5 * y * z >= xQx + sqrt(b)^2

                std::cerr << "Warning: Assuming \"" << head1 << " >= 0\" when converting quadratic constraint to Mosek expression." << std::endl;
                std::cerr << "Warning: Assuming \"" << head2 << " >= 0\" when converting quadratic constraint to Mosek expression." << std::endl;

                if (sign * rhs >= 0) {
                    throw Exception("Non-convex constraint was found.");
                }

                expression = mosek::fusion::AffExpr::vstack(
                        to_mosek_expression(head1),
                        to_mosek_expression(head2),
                        std::sqrt(-sign * rhs)
                );

            } else {
                throw Exception("Non-convex constraint was found.");
            }

        }

        for (const auto end = rq_cone_expr.end() ; it != end ; ++it) {
            expression = mosek::fusion::AffExpr::vstack(
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

     */

    // Build expression
    auto expr = to_mosek_expression(row);
    expr = mosek::fusion::Expr::add(std::move(expr), -rhs);

    // Set constraint type
    switch (type) {
        case LessOrEqual:
            result.constraint = m_model->constraint(/* t_ctr.name(), */ std::move(expr), mosek::fusion::Domain::lessThan(0.));
        break;
        case GreaterOrEqual:
            result.constraint = m_model->constraint(/* t_ctr.name(), */ std::move(expr), mosek::fusion::Domain::greaterThan(0.));
            break;
        case Equal:
            result.constraint = m_model->constraint(/* t_ctr.name(), */ std::move(expr), mosek::fusion::Domain::equalsTo(0.));
            break;
        default: throw Exception("Enum out of bounds.");
    }

    return result;
}

void idol::Optimizers::Mosek::hook_update_objective_sense() {
    hook_update_objective();
}

void idol::Optimizers::Mosek::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) {
    throw Exception("Not implemented hook_update_matrix");
}

void idol::Optimizers::Mosek::hook_update() {

}

void idol::Optimizers::Mosek::hook_update(const Var &t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const double obj = model.get_var_obj(t_var);

    set_var_attr(impl, type, lb, ub, obj);

}

void idol::Optimizers::Mosek::hook_update(const Ctr &t_ctr) {

    const double rhs = parent().get_ctr_rhs(t_ctr);

    auto msk_rhs = std::make_shared<monty::ndarray< double >>(monty::shape(1), -rhs);
    lazy(t_ctr).impl().constraint->index(0)->update(msk_rhs);

}

void idol::Optimizers::Mosek::hook_update_objective() {

    const auto& model = parent();
    const auto& objective = model.get_obj_expr();
    const int sense = model.get_obj_sense();

    auto expr = mosek::fusion::Expr::constTerm(objective.constant());

    for (const auto& [var, constant] : objective.linear()) {
        expr = mosek::fusion::Expr::add(
                std::move(expr),
                mosek::fusion::Expr::mul(
                        constant,
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

void idol::Optimizers::Mosek::hook_update_rhs() {
    throw Exception("Not implemented hook_update_rhs");
}

void idol::Optimizers::Mosek::hook_remove(const Var &t_var) {

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

void idol::Optimizers::Mosek::hook_remove(const Ctr &t_ctr) {
    auto& impl = lazy(t_ctr).impl();
    impl.constraint->remove();
    impl.constraint = nullptr;
}

void idol::Optimizers::Mosek::set_var_attr(MosekVar &t_mosek_var, int t_type, double t_lb, double t_ub, double t_obj) {

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

void idol::Optimizers::Mosek::set_var_lb(MosekVar &t_mosek_var, double t_bound) {

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

void idol::Optimizers::Mosek::set_var_ub(MosekVar &t_mosek_var, double t_bound) {

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

void idol::Optimizers::Mosek::set_param_time_limit(double t_time_limit) {
    m_model->setSolverParam("optimizerMaxTime", t_time_limit);
    Optimizer::set_param_time_limit(t_time_limit);
}

void idol::Optimizers::Mosek::set_param_threads(unsigned int t_thread_limit) {
    m_model->setSolverParam("numThreads", (int) t_thread_limit);
    Optimizer::set_param_threads(t_thread_limit);
}

void idol::Optimizers::Mosek::set_param_best_obj_stop(double t_best_obj_stop) {
    m_model->setSolverParam("upperObjCut", t_best_obj_stop);
    Optimizer::set_param_best_obj_stop(t_best_obj_stop);
}

void idol::Optimizers::Mosek::set_param_best_bound_stop(double t_best_bound_stop) {
    m_model->setSolverParam("lowerObjCut", t_best_bound_stop);
    Optimizer::set_param_best_bound_stop(t_best_bound_stop);
}

void idol::Optimizers::Mosek::set_param_presolve(bool t_value) {
    m_model->setSolverParam("presolveUse", t_value ? "on" : "off");
    Optimizer::set_param_presolve(t_value);
}

void idol::Optimizers::Mosek::set_param_infeasible_or_unbounded_info(bool t_value) {
    m_model->acceptedSolutionStatus(t_value ? mosek::fusion::AccSolutionStatus::Anything : mosek::fusion::AccSolutionStatus::Feasible);
    Optimizer::set_param_infeasible_or_unbounded_info(t_value);
}

idol::SolutionStatus idol::Optimizers::Mosek::get_status() const {
    return m_solution_status;
}

idol::SolutionReason idol::Optimizers::Mosek::get_reason() const {
    return m_solution_reason;
}

double idol::Optimizers::Mosek::get_best_obj() const {
    if (m_solution_status == Infeasible) { return Inf; }
    if (m_solution_status == Unbounded) { return -Inf; }
    return m_model->primalObjValue();
}

double idol::Optimizers::Mosek::get_best_bound() const {
    if (m_solution_status == Infeasible) { return Inf; }
    if (m_solution_status == Unbounded) { return -Inf; }
    return m_model->dualObjValue();
}

double idol::Optimizers::Mosek::get_var_primal(const Var &t_var) const {
    return lazy(t_var).impl().variable->level()->operator[](0);
}

double idol::Optimizers::Mosek::get_var_ray(const Var &t_var) const {
    if (m_model->getPrimalSolutionStatus() != mosek::fusion::SolutionStatus::Certificate) {
        throw Exception("Ray not available.");
    }
    return lazy(t_var).impl().variable->level()->operator[](0);
}

double idol::Optimizers::Mosek::get_ctr_dual(const Ctr &t_ctr) const {
    return lazy(t_ctr).impl().constraint->dual()->operator[](0);
}

double idol::Optimizers::Mosek::get_ctr_farkas(const Ctr &t_ctr) const {
    if (m_model->getDualSolutionStatus() != mosek::fusion::SolutionStatus::Certificate) {
        throw Exception("Farkas certificate not available.");
    }
    return lazy(t_ctr).impl().constraint->dual()->operator[](0);
}

double idol::Optimizers::Mosek::get_relative_gap() const {
    throw Exception("Not implemented.");
}

double idol::Optimizers::Mosek::get_absolute_gap() const {
    throw Exception("Not implemented.");
}

unsigned int idol::Optimizers::Mosek::get_n_solutions() const {
    const auto status = get_status();
    return status == Optimal || status == Feasible;
}

unsigned int idol::Optimizers::Mosek::get_solution_index() const {
    return 0;
}

void idol::Optimizers::Mosek::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds.");
    }
}

void idol::Optimizers::Mosek::set_parameter(const std::string &t_param, double t_value) {
    m_model->setSolverParam(t_param, t_value);
}

void idol::Optimizers::Mosek::set_parameter(const std::string &t_param, int t_value) {
    m_model->setSolverParam(t_param, t_value);
}

void idol::Optimizers::Mosek::set_parameter(const std::string &t_param, const std::string &t_value) {
    m_model->setSolverParam(t_param, t_value);
}

void idol::Optimizers::Mosek::set_param_logs(bool t_value) {

    if (!t_value) {
        m_model->setLogHandler(nullptr);
    } else {
        m_model->setLogHandler([this](const std::string& t_msg){
            std::cout << t_msg << std::endl;
        });
    }

    Optimizer::set_param_logs(t_value);
}

double idol::Optimizers::Mosek::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

void idol::Optimizers::Mosek::add_callback(idol::Callback *t_ptr_to_callback) {

    if (!m_mosek_callback) {

        m_mosek_callback = std::make_unique<MosekCallbackI>(*this);

        auto cb =
            [&](MSKcallbackcodee caller, const double * t_double_info, const int32_t* t_int32_info, const int64_t* t_int64_info) {
                m_mosek_callback->callback(caller, t_double_info, t_int32_info, t_int64_info);
                return is_terminated();
            };

        m_model->setDataCallbackHandler(cb);
    }

    m_mosek_callback->add_callback(t_ptr_to_callback);
}

idol::MosekKiller::~MosekKiller() {
    mosek::releaseGlobalEnv();
}

#endif
