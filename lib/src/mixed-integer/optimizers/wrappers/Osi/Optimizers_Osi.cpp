//
// Created by henri on 05.02.24.
//

#ifdef IDOL_USE_OSI

#include "idol/mixed-integer/optimizers/wrappers/Osi/Optimizers_Osi.h"
#include "CoinPackedVector.hpp"

idol::Optimizers::Osi::Osi(const idol::Model &t_model,
                           const OsiSolverInterface &t_solver_interface,
                           bool t_continuous_relaxation)
                           : OptimizerWithLazyUpdates<int, int>(t_model),
                             m_solver_interface(t_solver_interface.clone()),
                             m_continuous_relaxation(t_continuous_relaxation) {

    m_solver_interface->messageHandler()->setLogLevel(get_param_logs());

    // TODO, make this optional
    m_solver_interface->setHintParam(OsiHintParam::OsiDoDualInInitial, false, OsiHintStrength::OsiHintDo);
    m_solver_interface->setHintParam(OsiHintParam::OsiDoDualInResolve, true, OsiHintStrength::OsiHintDo);


}

std::string idol::Optimizers::Osi::name() const {
    return "osi";
}

idol::SolutionStatus idol::Optimizers::Osi::get_status() const {

    if (m_solver_interface->isAbandoned()) {
        return Fail;
    }

    if (m_solver_interface->isProvenOptimal()) {
        return Optimal;
    }

    if (m_solver_interface->isProvenDualInfeasible()) {
        return Unbounded;
    }

    if (m_solver_interface->isProvenPrimalInfeasible()) {
        return Infeasible;
    }

    if (m_solver_interface->isPrimalObjectiveLimitReached()) {
        return Feasible;
    }

    if (m_solver_interface->isDualObjectiveLimitReached()) {
        return SubOptimal;
    }

    return Infeasible; // TODO This is done because MILPs do not have a proper status when infeasible... at least for Cplex

    throw Exception("Osi::get_status could not infer status.");
}

idol::SolutionReason idol::Optimizers::Osi::get_reason() const {

    if (m_solver_interface->isIterationLimitReached()) {
        return IterLimit;
    }

    if (m_solver_interface->isProvenOptimal() || m_solver_interface->isProvenPrimalInfeasible() || m_solver_interface->isProvenDualInfeasible()) {
        return Proved;
    }

    if (m_solver_interface->isPrimalObjectiveLimitReached()) {
        return ObjLimit;
    }

    return NotSpecified;

}

double idol::Optimizers::Osi::get_best_obj() const {

    const auto status = get_status();

    if (status == Infeasible) {
        return Inf;
    }

    if (status == Unbounded) {
        return -Inf;
    }

    const auto& objective = parent().get_obj_expr();
    return objective.constant() + m_solver_interface->getObjValue();
}

double idol::Optimizers::Osi::get_best_bound() const {
    return get_best_obj(); // TODO handle this properly...
}

double idol::Optimizers::Osi::get_var_primal(const idol::Var &t_var) const {
    return m_solver_interface->getColSolution()[lazy(t_var).impl()];
}

double idol::Optimizers::Osi::get_var_ray(const idol::Var &t_var) const {
    return m_solver_interface->getPrimalRays(1).front()[lazy(t_var).impl()];
}

double idol::Optimizers::Osi::get_ctr_dual(const idol::Ctr &t_ctr) const {
    return m_solver_interface->getRowPrice()[lazy(t_ctr).impl()];
}

double idol::Optimizers::Osi::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    return -m_solver_interface->getDualRays(1).front()[lazy(t_ctr).impl()];
}

double idol::Optimizers::Osi::get_relative_gap() const {
    throw Exception("Not implemented Osi::get_relative_gap");
}

double idol::Optimizers::Osi::get_absolute_gap() const {
    throw Exception("Not implemented Osi::get_absolute_gap");
}

unsigned int idol::Optimizers::Osi::get_n_solutions() const {
    if (m_solver_interface->isProvenOptimal()) {
        return 1;
    }
    return 0;
}

unsigned int idol::Optimizers::Osi::get_solution_index() const {
    return 0;
}

void idol::Optimizers::Osi::hook_optimize() {

    const bool presolve = get_param_presolve();
    m_solver_interface->setHintParam(OsiHintParam::OsiDoPresolveInInitial, presolve, OsiHintStrength::OsiHintDo);
    m_solver_interface->setHintParam(OsiHintParam::OsiDoPresolveInResolve, presolve, OsiHintStrength::OsiHintDo);

    m_solver_interface->initialSolve(); // TODO handle resolve

    if (m_solver_interface->isProvenOptimal()) {

        if (m_continuous_relaxation || m_solver_interface->getNumIntegers() == 0) {
            return;
        }

        m_solver_interface->branchAndBound();
        return;

    }

    if (m_solver_interface->isProvenPrimalInfeasible() || m_solver_interface->isProvenDualInfeasible()){

        if (!get_param_infeasible_or_unbounded_info()) {
            return;
        }

        if (get_param_presolve()) {
            set_param_presolve(false);
            m_solver_interface->initialSolve();
            return;
        }

        return;
    }

}

void idol::Optimizers::Osi::set_solution_index(unsigned int t_index) {
    if (t_index > 0) {
        throw Exception("Index out of bounds.");
    }
}

void idol::Optimizers::Osi::hook_build() {

    const auto& objective = parent().get_obj_expr();

    if (!objective.quadratic().empty()) {
        throw Exception("Osi is not available as an SOCP solver.");
    }

    hook_update_objective_sense();
    set_objective_as_updated();
    set_rhs_as_updated();

}

void idol::Optimizers::Osi::hook_write(const std::string &t_name) {
    const std::string extension = t_name.substr(t_name.find_last_of('.') + 1);

    if (extension.empty()) {
        m_solver_interface->writeLp(t_name.c_str(), ".lp");
        return;
    }

    if (extension == "lp") {
        m_solver_interface->writeLp(t_name.c_str(), "");
        return;
    }

    if (extension == "mps") {
        m_solver_interface->writeMps(t_name.c_str(), "");
        return;
    }

    throw Exception("Osi support for " + extension + "-files is not implemented.");
}

int idol::Optimizers::Osi::hook_add(const idol::Var &t_var, bool t_add_column) {

    const int index = (int) m_solver_interface->getNumCols();

    double lb = parent().get_var_lb(t_var);
    double ub = parent().get_var_ub(t_var);
    const auto& column = parent().get_var_column(t_var);
    const auto type = parent().get_var_type(t_var);

    CoinPackedVector vector;

    if (t_add_column) {

        if (!column.quadratic().empty()) {
            throw Exception("Osi cannot handle quadratic expressions.");
        }

        for (const auto& [ctr, constant] : column.linear()) {
            const int ctr_index = lazy(ctr).impl();
            const double coefficient = constant;
            vector.insert(ctr_index, coefficient);
        }

    }

    if (type == Binary) {
        lb = std::max(0., lb);
        ub = std::min(1., ub);
    }

    m_solver_interface->addCol(vector, lb, ub, column.obj(), t_var.name());

    if (type == Binary || type == Integer) {
        m_solver_interface->setInteger(index);
    }

    return index;
}

int idol::Optimizers::Osi::hook_add(const idol::Ctr &t_ctr) {

    const int index = m_solver_interface->getNumRows();

    const auto& row = parent().get_ctr_row(t_ctr);
    const double rhs = row.rhs();
    const auto type = parent().get_ctr_type(t_ctr);

    if (!row.quadratic().empty()) {
        throw Exception("Osi cannot handle quadratic expressions.");
    }

    CoinPackedVector vector;
    for (const auto& [var, coeff] : row.linear()) {
        const int var_index = lazy(var).impl();
        const double coefficient = coeff;
        vector.insert(var_index, coefficient);
    }

    double lb = -Inf, ub = Inf;
    switch (type) {
        case LessOrEqual: ub = rhs; break;
        case GreaterOrEqual: lb = rhs; break;
        case Equal: ub = lb = rhs; break;
    }

    m_solver_interface->addRow(vector, lb, ub, t_ctr.name());

    return index;
}

void idol::Optimizers::Osi::hook_update_objective_sense() {
    const auto sense = parent().get_obj_sense();
    m_solver_interface->setObjSense(sense == Minimize ? 1. : -1.);
}

void idol::Optimizers::Osi::hook_update_matrix(const idol::Ctr &t_ctr, const idol::Var &t_var, double t_constant) {
    throw Exception("Not implemented Osi::hook_update_matrix");
}

void idol::Optimizers::Osi::hook_update() {

}

void idol::Optimizers::Osi::hook_update(const idol::Var &t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    double lb = model.get_var_lb(t_var);
    double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const double obj = model.get_var_column(t_var).obj();

    if (type == Binary) {
        lb = std::max(0., lb);
        ub = std::min(1., ub);
    }

    m_solver_interface->setColLower(impl, lb);
    m_solver_interface->setColUpper(impl, ub);

    if (type == Continuous) {
        if (!m_solver_interface->isContinuous(impl)) {
            m_solver_interface->setContinuous(impl);
        }
    } else if (type == Integer || type == Binary) {
        if (!m_solver_interface->isInteger(impl)) {
            m_solver_interface->setInteger(impl);
        }
    }

    m_solver_interface->setObjCoeff(impl, obj);

}

void idol::Optimizers::Osi::hook_update(const idol::Ctr &t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();
    const auto& rhs = model.get_ctr_row(t_ctr).rhs();
    const auto type = model.get_ctr_type(t_ctr);

    double lb = -Inf, ub = Inf;
    switch (type) {
        case LessOrEqual: ub = rhs; break;
        case GreaterOrEqual: lb = rhs; break;
        case Equal: ub = lb = rhs; break;
    }

    m_solver_interface->setRowBounds(impl, lb, ub);

}

void idol::Optimizers::Osi::hook_update_objective() {

    const auto& parent = this->parent();

    if (!parent.get_obj_expr().quadratic().empty()) {
        throw Exception("Osi is not available as an SOCP solver.");
    }

    const unsigned int n_variables = parent.vars().size();

    auto* coefficients = new double[n_variables];
    for (const auto& var : parent.vars()) {
        const unsigned int index = lazy(var).impl();
        coefficients[index] = parent.get_var_column(var).obj();
    }
    m_solver_interface->setObjective(coefficients);

    delete[] coefficients;

}

void idol::Optimizers::Osi::hook_update_rhs() {
    throw Exception("Not implemented Osi::hook_update_rhs");
}

void idol::Optimizers::Osi::hook_remove(const idol::Var &t_var) {

    const int index = lazy(t_var).impl();

    m_solver_interface->deleteCols(1, &index);

    for (auto& lazy : lazy_vars()) {
        if (lazy.impl() > index) {
            lazy.impl() -= 1;
        }
    }

}

void idol::Optimizers::Osi::hook_remove(const idol::Ctr &t_ctr) {

    const int index = lazy(t_ctr).impl();

    m_solver_interface->deleteCols(1, &index);

    for (auto& lazy : lazy_ctrs()) {
        if (lazy.impl() > index) {
            lazy.impl() -= 1;
        }
    }

}

void idol::Optimizers::Osi::set_param_logs(bool t_value) {
    Optimizer::set_param_logs(t_value);
    m_solver_interface->messageHandler()->setLogLevel((int) t_value);
}

void idol::Optimizers::Osi::set_param_presolve(bool t_value) {
    Optimizer::set_param_presolve(t_value);
    m_solver_interface->setHintParam(OsiHintParam::OsiDoPresolveInInitial, t_value, OsiHintStrength::OsiHintDo);
    m_solver_interface->setHintParam(OsiHintParam::OsiDoPresolveInResolve, t_value, OsiHintStrength::OsiHintDo);
}

double idol::Optimizers::Osi::get_var_reduced_cost(const idol::Var &t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

#endif
