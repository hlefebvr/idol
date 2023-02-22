#ifdef IDOL_USE_MOSEK

#include "backends/solvers/Mosek.h"
#include "linear-algebra/to_rotated_quadratic_cone.h"
#include <Eigen/Sparse>

Mosek::Mosek(const AbstractModel &t_model)
    : LazyBackend<MosekVar, MosekCtr>(t_model),
      m_model(new mosek::fusion::Model()) {

    m_model->setLogHandler(NULL);

}

Mosek::~Mosek() {
    m_model->dispose();
}

void Mosek::hook_initialize() {

    const auto& objective = parent().get(Attr::Obj::Expr);

    if (!objective.quadratic().empty()) {
        throw Exception("Handling quadratic objective is not implemented.");
    }

    hook_update_objective_sense();
    set_objective_as_updated();
    set_rhs_as_updated();
}

void Mosek::hook_optimize() {

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

    } else {
        std::cout << problem_status << ", " << primal_status << ", " << dual_status << std::endl;
        throw Exception("Unknown problem-solution status combination.");
    }
}

void Mosek::hook_write(const std::string &t_name) {
    m_model->writeTask(t_name);
}

MosekVar Mosek::hook_add(const Var &t_var, bool t_add_column) {

    MosekVar result;

    result.variable = m_model->variable(1, mosek::fusion::Domain::unbounded());

    const double lb = parent().get(Attr::Var::Lb, t_var);
    const double ub = parent().get(Attr::Var::Ub, t_var);
    const auto& column = parent().get(Attr::Var::Column, t_var);
    const auto type = parent().get(Attr::Var::Type, t_var);

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

mosek::fusion::Expression::t Mosek::to_mosek_expression(const LinExpr<Var> &t_expr) const {

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

MosekCtr Mosek::hook_add(const Ctr &t_ctr) {

    MosekCtr result;

    const auto& model = parent();

    const int type = model.get(Attr::Ctr::Type, t_ctr);
    const auto& row = model.get(Attr::Ctr::Row, t_ctr);

    if (!row.quadratic().empty()) {

#ifdef IDOL_USE_EIGEN

        auto rq_cone_expr = to_rotated_quadratic_cone(row.quadratic());

        auto expression = mosek::fusion::Expr::zeros(0);

        auto it = rq_cone_expr.begin();

        if (!row.linear().empty()) {

            assert(it->empty());
            ++it;
            assert(it->empty());

            auto linear_part = row.linear();
            linear_part *= -1.;

            expression = mosek::fusion::Expr::vstack(
                        std::move(expression),
                        mosek::fusion::Expr::constTerm(.5),
                        mosek::fusion::Expr::add(as_numeric(row.rhs()), to_mosek_expression(linear_part))
                    );

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

void Mosek::hook_update_objective_sense() {
    std::cout << "skips updating sense" << std::endl;
}

void Mosek::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {
    throw Exception("Not implemented hook_update_matrix");
}

void Mosek::hook_update() {

}

void Mosek::hook_update(const Var &t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get(Attr::Var::Lb, t_var);
    const double ub = model.get(Attr::Var::Ub, t_var);
    const int type = model.get(Attr::Var::Type, t_var);
    const Constant& obj = model.get(Attr::Var::Obj, t_var);

    set_var_attr(impl, type, lb, ub, as_numeric(obj));

}

void Mosek::hook_update(const Ctr &t_ctr) {

    const auto& row = parent().get(Attr::Ctr::Row, t_ctr);

    auto rhs = std::make_shared<monty::ndarray< double >>(monty::shape(1), -as_numeric(row.rhs()));
    lazy(t_ctr).impl().constraint->index(0)->update(rhs);

}

void Mosek::hook_update_objective() {

    const auto& model = parent();
    const auto& objective = model.get(Attr::Obj::Expr);
    const int sense = model.get(Attr::Obj::Sense);

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

void Mosek::hook_update_rhs() {
    throw Exception("Not implemented hook_update_rhs");
}

void Mosek::hook_remove(const Var &t_var) {

    auto& impl = lazy(t_var).impl();

    m_model->updateObjective(mosek::fusion::Expr::constTerm(0), impl.variable);

    impl.variable->remove();

    if (impl.lower_bound.get()) {
        impl.lower_bound->remove();
        impl.lower_bound = nullptr;
    }

    if (impl.upper_bound.get()) {
        impl.upper_bound->remove();
        impl.upper_bound = nullptr;
    }
}

void Mosek::hook_remove(const Ctr &t_ctr) {
    auto& impl = lazy(t_ctr).impl();
    impl.constraint->remove();
}

void Mosek::set_var_attr(MosekVar &t_mosek_var, int t_type, double t_lb, double t_ub, double t_obj) {

    // Set obj
    m_model->updateObjective(mosek::fusion::Expr::mul(t_obj, t_mosek_var.variable), t_mosek_var.variable);

    // Set type
    switch (t_type) {
        case Continuous: t_mosek_var.variable->make_continuous(); break;
        case Binary: [[fallthrough]];
        case Integer: t_mosek_var.variable->make_integer(); break;
        default: throw std::runtime_error("Unknown variable type.");
    }

    // Set bounds
    set_var_lb(t_mosek_var, t_lb);
    set_var_ub(t_mosek_var, t_ub);
}

void Mosek::set_var_lb(MosekVar &t_mosek_var, double t_bound) {

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

void Mosek::set_var_ub(MosekVar &t_mosek_var, double t_bound) {

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

int Mosek::get(const Req<int, void> &t_attr) const {

    if (t_attr == Attr::Solution::Status) {
        return m_solution_status;
    }

    if (t_attr == Attr::Solution::Reason) {
        return m_solution_reason;
    }

    return Base::get(t_attr);
}

double Mosek::get(const Req<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        if (m_solution_status == Infeasible) { return Inf; }
        if (m_solution_status == Unbounded) { return -Inf; }
        return m_model->primalObjValue();
    }

    return Base::get(t_attr);
}

double Mosek::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Solution::Primal) {
        return lazy(t_var).impl().variable->level()->operator[](0);
    }

    if (t_attr == Attr::Solution::Ray) {
        if (m_model->getPrimalSolutionStatus() != mosek::fusion::SolutionStatus::Certificate) {
            throw Exception("Not available.");
        }
        return lazy(t_var).impl().variable->level()->operator[](0);
    }

    return Base::get(t_attr, t_var);
}

double Mosek::get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Solution::Dual) {
        return lazy(t_ctr).impl().constraint->dual()->operator[](0);
    }

    if (t_attr == Attr::Solution::Farkas) {
        if (m_model->getDualSolutionStatus() != mosek::fusion::SolutionStatus::Certificate) {
            throw Exception("Not available.");
        }
        return lazy(t_ctr).impl().constraint->dual()->operator[](0);
    }

    return Base::get(t_attr, t_ctr);
}

double Mosek::get(const Parameter<double> &t_param) const {

    if (t_param == Param::Algorithm::BestBoundStop) {
        return m_model->getSolverDoubleInfo("lowerObjCut");
    }

    if (t_param == Param::Algorithm::BestObjStop) {
        return m_model->getSolverDoubleInfo("upperObjCut");
    }

    if (t_param == Param::Algorithm::TimeLimit) {
        return m_model->getSolverDoubleInfo("optimizerMaxTime");
    }

    return Base::get(t_param);
}

void Mosek::set(const Parameter<double> &t_param, double t_value) {

    if (t_param == Param::Algorithm::BestBoundStop) {
        m_model->setSolverParam("lowerObjCut", t_value);
        return;
    }

    if (t_param == Param::Algorithm::BestObjStop) {
        m_model->setSolverParam("upperObjCut", t_value);
        return;
    }

    if (t_param == Param::Algorithm::TimeLimit) {
        m_model->setSolverParam("optimizerMaxTime", t_value);
        return;
    }

    Base::set(t_param, t_value);
}

void Mosek::set(const Parameter<bool> &t_param, bool t_value) {

    if (t_param == Param::Algorithm::InfeasibleOrUnboundedInfo) {
        m_model->acceptedSolutionStatus(t_value ? mosek::fusion::AccSolutionStatus::Anything : mosek::fusion::AccSolutionStatus::Feasible);
        return;
    }

    Base::set(t_param, t_value);
}

bool Mosek::get(const Parameter<bool> &t_param) const {

    if (t_param == Param::Algorithm::InfeasibleOrUnboundedInfo) {
        return m_model->getAcceptedSolutionStatus() == mosek::fusion::AccSolutionStatus::Anything;
    }

    return Base::get(t_param);
}


#endif