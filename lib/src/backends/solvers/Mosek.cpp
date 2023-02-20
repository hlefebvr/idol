#include "backends/solvers/Mosek.h"

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
}

void Mosek::hook_write(const std::string &t_name) {
    m_model->writeTask(t_name);
}

MosekVar Mosek::hook_add(const Var &t_var, bool t_add_column) {

    MosekVar result;

    result.variable = m_model->variable(t_var.name(), 1, mosek::fusion::Domain::unbounded());

    const double lb = parent().get(Attr::Var::Lb, t_var);
    const double ub = parent().get(Attr::Var::Ub, t_var);
    const auto& column = parent().get(Attr::Var::Column, t_var);
    const auto type = parent().get(Attr::Var::Type, t_var);

    set_var_attr(result, type, lb, ub, as_numeric(column.obj()));

    if (t_add_column) {
        throw Exception("Not implemented: add variable by column.");
    }

    return result;

}

MosekCtr Mosek::hook_add(const Ctr &t_ctr) {

    MosekCtr result;

    const auto& model = parent();

    const int type = model.get(Attr::Ctr::Type, t_ctr);
    const auto& row = model.get(Attr::Ctr::Row, t_ctr);

    // Build expression
    auto expr = mosek::fusion::Expr::constTerm(-as_numeric(row.rhs()));

    for (const auto& [var, constant] : row.linear()) {
        expr = mosek::fusion::Expr::add(
                std::move(expr),
                mosek::fusion::Expr::mul(
                        as_numeric(constant),
                        lazy(var).impl().variable
                    )
            );
    }

    // Set constraint type
    switch (type) {
        case LessOrEqual:
            result.constraint = m_model->constraint(t_ctr.name(), std::move(expr), mosek::fusion::Domain::lessThan(0.));
        break;
        case GreaterOrEqual:
            result.constraint = m_model->constraint(t_ctr.name(), std::move(expr), mosek::fusion::Domain::greaterThan(0.));
            break;
        case Equal:
            result.constraint = m_model->constraint(t_ctr.name(), std::move(expr), mosek::fusion::Domain::equalsTo(0.));
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
    throw Exception("Not implemented hook_update");
}

void Mosek::hook_update_objective() {
    throw Exception("Not implemented hook_update_objective");
}

void Mosek::hook_update_rhs() {
    throw Exception("Not implemented hook_update_rhs");
}

void Mosek::hook_remove(const Var &t_var) {
    throw Exception("Not implemented hook_remove");
}

void Mosek::hook_remove(const Ctr &t_ctr) {
    throw Exception("Not implemented hook_remove");
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
        auto problem_status = m_model->getProblemStatus();
        auto primal_status = m_model->getPrimalSolutionStatus();
        auto dual_status = m_model->getDualSolutionStatus();

        if (problem_status == mosek::fusion::ProblemStatus::PrimalAndDualFeasible
            && primal_status == mosek::fusion::SolutionStatus::Optimal
            && dual_status == mosek::fusion::SolutionStatus::Optimal) {
            return Optimal;
        } else if(problem_status == mosek::fusion::ProblemStatus::PrimalFeasible
                  && primal_status == mosek::fusion::SolutionStatus::Optimal
                  && dual_status == mosek::fusion::SolutionStatus::Undefined) {
            return Optimal;
        } else if (problem_status == mosek::fusion::ProblemStatus::PrimalInfeasible
                  && primal_status == mosek::fusion::SolutionStatus::Undefined
                  && dual_status == mosek::fusion::SolutionStatus::Certificate) {
            return Infeasible;
        } else {
            std::cout << problem_status << ", " << primal_status << ", " << dual_status << std::endl;
            throw Exception("Unknown problem-solution status combination.");
        }
    }

    if (t_attr == Attr::Solution::Reason) {
        return Proved;
    }

    return Base::get(t_attr);
}

double Mosek::get(const Req<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        return m_model->primalObjValue();
    }

    return Base::get(t_attr);
}

double Mosek::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Solution::Primal) {
        return lazy(t_var).impl().variable->level()->operator[](0);
    }

    return Base::get(t_attr, t_var);
}

double Mosek::get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const {

    if (t_attr == Attr::Solution::Dual) {
        return lazy(t_ctr).impl().constraint->level()->operator[](0);
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
