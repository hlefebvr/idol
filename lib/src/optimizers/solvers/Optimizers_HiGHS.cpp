//
// Created by henri on 14/02/23.
//
#include "idol/optimizers/solvers/Optimizers_HiGHS.h"
#include "idol/optimizers/Algorithm.h"

#ifdef IDOL_USE_HIGHS

idol::Optimizers::HiGHS::HiGHS(const Model &t_model, bool t_continuous_relaxation)
    : OptimizerWithLazyUpdates(t_model),
      m_continuous_relaxation(t_continuous_relaxation) {

    m_model.setOptionValue("output_flag", false);

}

void idol::Optimizers::HiGHS::hook_build() {

    const auto& objective = parent().get_obj_expr();

    if (!objective.quadratic().empty()) {
        throw Exception("HiGHS is not available as an SOCP solver.");
    }

    hook_update_objective_sense();
    set_objective_as_updated();
    set_rhs_as_updated();

}

void idol::Optimizers::HiGHS::hook_write(const std::string &t_name) {
    m_model.writeModel(t_name);
}

void idol::Optimizers::HiGHS::set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj) {

    const bool has_lb = !is_neg_inf(t_lb);
    const bool has_ub = !is_pos_inf(t_ub);

    // Set obj
    m_model.changeColCost(t_index, t_obj);

    // Set type
    if (m_continuous_relaxation || t_type == Continuous) {
        m_model.changeColIntegrality(t_index, HighsVarType::kContinuous);
    } else if (t_type == Integer || t_type == Binary) {
        m_model.changeColIntegrality(t_index, HighsVarType::kInteger);
    } else {
        throw std::runtime_error("Unknown variable type.");
    }

    // Set bounds
    if (has_lb && has_ub) {
        m_model.changeColBounds(t_index, t_lb, t_ub);
    } else if (has_lb) {
        m_model.changeColBounds(t_index, t_lb, m_model.getInfinity());
    } else if (has_ub) {
        m_model.changeColBounds(t_index, -m_model.getInfinity(), t_ub);
    } else {
        m_model.changeColBounds(t_index, -m_model.getInfinity(), m_model.getInfinity());
    }

}

int idol::Optimizers::HiGHS::hook_add(const Var &t_var, bool t_add_column) {

    int index;
    if (m_deleted_variables.empty()) {
        index = (int) m_model.getNumCol();
        m_model.addCol(0., 0., 0., 0, nullptr, nullptr);
    } else {
        index = m_deleted_variables.top();
        m_deleted_variables.pop();
    }

    const double lb = parent().get_var_lb(t_var);
    const double ub = parent().get_var_ub(t_var);
    const auto& column = parent().get_var_column(t_var);
    const auto type = parent().get_var_type(t_var);

    set_var_attr(index, type, lb, ub, as_numeric(column.obj()));

    if (t_add_column) {

        for (const auto& [ctr, constant] : column.linear()) {
            m_model.changeCoeff(lazy(ctr).impl(), index, as_numeric(constant));
        }

    }

    return index;
}

void idol::Optimizers::HiGHS::set_ctr_attr(int t_index, int t_type, double t_rhs) {

    switch (t_type) {
        case LessOrEqual:
            m_model.changeRowBounds(t_index, -m_model.getInfinity(), t_rhs);
            break;
        case GreaterOrEqual:
            m_model.changeRowBounds(t_index, t_rhs, m_model.getInfinity());
            break;
        case Equal:
            m_model.changeRowBounds(t_index, t_rhs, t_rhs);
            break;
        default:
            throw std::runtime_error("Unknown constraint type.");
    }

}

int idol::Optimizers::HiGHS::hook_add(const Ctr &t_ctr) {

    int index;
    if (m_deleted_constraints.empty()) {
        index = (int) m_model.getNumRow();
        m_model.addRow(0., 0., 0, nullptr, nullptr);
    } else {
        index = m_deleted_constraints.top();
        m_deleted_constraints.pop();
    }

    const auto& row = parent().get_ctr_row(t_ctr);
    const double rhs = as_numeric(row.rhs());
    const auto type = parent().get_ctr_type(t_ctr);

    set_ctr_attr(index, type, rhs);

    for (const auto& [var, coeff] : row.linear()) {
        m_model.changeCoeff(index, lazy(var).impl(), as_numeric(coeff));
    }

    return index;
}

void idol::Optimizers::HiGHS::hook_update_objective_sense() {
    m_model.changeObjectiveSense(parent().get_obj_sense() == Minimize ? ObjSense::kMinimize : ObjSense::kMaximize );
}

void idol::Optimizers::HiGHS::hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) {
    const auto& coeff = parent().get_mat_coeff(t_ctr, t_var);
    m_model.changeCoeff(lazy(t_ctr).impl(), lazy(t_var).impl(), as_numeric(coeff));
}

void idol::Optimizers::HiGHS::hook_update() {

}

void idol::Optimizers::HiGHS::hook_update(const Var &t_var) {

    const auto& model = parent();
    auto& impl = lazy(t_var).impl();
    const double lb = model.get_var_lb(t_var);
    const double ub = model.get_var_ub(t_var);
    const int type = model.get_var_type(t_var);
    const Constant& obj = model.get_var_column(t_var).obj();

    set_var_attr(impl, type, lb, ub, as_numeric(obj));

}

void idol::Optimizers::HiGHS::hook_update(const Ctr &t_ctr) {

    const auto& model = parent();
    auto& impl = lazy(t_ctr).impl();
    const auto& rhs = model.get_ctr_row(t_ctr).rhs();
    const auto type = model.get_ctr_type(t_ctr);

    set_ctr_attr(impl, type, as_numeric(rhs));
}

void idol::Optimizers::HiGHS::hook_update_objective() {

    const auto& model = parent();

    for (const auto& var : model.vars()) {
        const auto& obj = model.get_var_column(var).obj();
        m_model.changeColCost(lazy(var).impl(), as_numeric(obj));
    }

    m_model.changeObjectiveOffset(as_numeric(model.get_obj_expr().constant()));

}

void idol::Optimizers::HiGHS::hook_update_rhs() {
    throw Exception("Not implemented.");
}

void idol::Optimizers::HiGHS::hook_remove(const Var &t_var) {

    const int index = lazy(t_var).impl();

    m_model.changeColCost(index, 0.);
    m_model.changeColBounds(index, 0., 0.);
    for (const auto& [ctr, constant] : parent().get_var_column(t_var).linear()) {
        m_model.changeCoeff(lazy(ctr).impl(), index, 0.);
    }
    m_deleted_variables.push(index);

}

void idol::Optimizers::HiGHS::hook_remove(const Ctr &t_ctr) {

    const int index = lazy(t_ctr).impl();

    m_model.changeRowBounds(index, 0., 0.);
    for (const auto& [var, constant] : parent().get_ctr_row(t_ctr).linear()) {
        m_model.changeCoeff(index, lazy(var).impl(), 0.);
    }
    m_deleted_constraints.push(index);

}

void idol::Optimizers::HiGHS::hook_optimize() {

    delete m_extreme_ray;
    m_extreme_ray = nullptr;

    delete m_farkas_certificate;
    m_farkas_certificate = nullptr;

    m_model.run();

    const auto status = m_model.getModelStatus();

    switch (status) {
        case HighsModelStatus::kNotset:
            m_solution_status = Loaded;
            m_solution_reason = NotSpecified;
            break;
        case HighsModelStatus::kSolveError:
        case HighsModelStatus::kPostsolveError:
        case HighsModelStatus::kLoadError:
        case HighsModelStatus::kModelError:
        case HighsModelStatus::kPresolveError:
        case HighsModelStatus::kUnknown:
            m_solution_status = Fail;
            m_solution_reason = NotSpecified;
            break;
        case HighsModelStatus::kModelEmpty:
        case HighsModelStatus::kOptimal:
            m_solution_status = Optimal;
            m_solution_reason = Proved;
            break;
        case HighsModelStatus::kInfeasible:
            m_solution_status = Infeasible;
            m_solution_reason = Proved;
            break;
        case HighsModelStatus::kUnboundedOrInfeasible:
            m_solution_status = InfOrUnbnd;
            m_solution_reason = Proved;
            break;
        case HighsModelStatus::kUnbounded:
            m_solution_status = Unbounded;
            m_solution_reason = Proved;
            break;
        case HighsModelStatus::kObjectiveBound:
        case HighsModelStatus::kObjectiveTarget:
        case HighsModelStatus::kTimeLimit:
        case HighsModelStatus::kIterationLimit:
        case HighsModelStatus::kSolutionLimit:
        case HighsModelStatus::kInterrupt:
            throw Exception("Unhandled status: " + std::to_string((int) status));
    }

    if (get_param_infeasible_or_unbounded_info() && m_solution_status == Unbounded) {

        bool has_primal_ray;
        m_extreme_ray = new double[m_model.getNumCol()];
        m_model.getPrimalRay(has_primal_ray, m_extreme_ray);

        if (!has_primal_ray) {
            run_without_presolve();
            m_model.getPrimalRay(has_primal_ray, m_extreme_ray);
            if (!has_primal_ray) {
                throw Exception("Cannot save primal ray.");
            }
        }

    }

    if (get_param_infeasible_or_unbounded_info() && m_solution_status == Infeasible) {

        bool has_dual_ray;
        m_farkas_certificate = new double[m_model.getNumRow()];
        m_model.getDualRay(has_dual_ray, m_farkas_certificate);

        if (!has_dual_ray) {
            run_without_presolve();
            m_model.getDualRay(has_dual_ray, m_farkas_certificate);
            if (!has_dual_ray) {
                throw Exception("Cannot save dual ray.");
            }
        }

    }

}

void idol::Optimizers::HiGHS::run_without_presolve() {
    const std::string old_presolve_setting;
    m_model.getStringOptionValues("presolve");
    m_model.setOptionValue("presolve", "off");
    m_model.run();
    m_model.setOptionValue("presolve", old_presolve_setting);
}

void idol::Optimizers::HiGHS::set_param_time_limit(double t_time_limit) {
    m_model.setOptionValue("time_limit", t_time_limit);
    Optimizer::set_param_time_limit(t_time_limit);
}

void idol::Optimizers::HiGHS::set_param_best_obj_stop(double t_best_obj_stop) {
    m_model.setOptionValue("objective_bound", t_best_obj_stop);
    Optimizer::set_param_best_obj_stop(t_best_obj_stop);
}

void idol::Optimizers::HiGHS::set_param_best_bound_stop(double t_best_bound_stop) {
    m_model.setOptionValue("objective_target", t_best_bound_stop);
    Optimizer::set_param_best_bound_stop(t_best_bound_stop);
}

void idol::Optimizers::HiGHS::set_param_presolve(bool t_value) {
    m_model.setOptionValue("presolve", t_value);
    Optimizer::set_param_presolve(t_value);
}

idol::SolutionStatus idol::Optimizers::HiGHS::get_status() const {
    return m_solution_status;
}

idol::SolutionReason idol::Optimizers::HiGHS::get_reason() const {
    return m_solution_reason;
}

double idol::Optimizers::HiGHS::get_best_obj() const {

    if (m_solution_status == Unbounded) {
        return -Inf;
    }

    if (m_solution_status == Infeasible) {
        return +Inf;
    }

    return m_model.getObjectiveValue();
}

double idol::Optimizers::HiGHS::get_best_bound() const {
    return get_best_obj();
}

double idol::Optimizers::HiGHS::get_var_primal(const Var &t_var) const {
    return m_model.getSolution().col_value[lazy(t_var).impl()];
}

double idol::Optimizers::HiGHS::get_var_ray(const Var &t_var) const {

    if (!m_extreme_ray) {
        throw Exception("Extreme ray is not available.");
    }

    return m_extreme_ray[lazy(t_var).impl()];
}

double idol::Optimizers::HiGHS::get_ctr_dual(const Ctr &t_ctr) const {
    return m_model.getSolution().row_dual[lazy(t_ctr).impl()];
}

double idol::Optimizers::HiGHS::get_ctr_farkas(const Ctr &t_ctr) const {

    if (!m_farkas_certificate) {
        throw Exception("Farkas certificate not available.");
    }

    return m_farkas_certificate[lazy(t_ctr).impl()];
}

double idol::Optimizers::HiGHS::get_relative_gap() const {
    return 0;
}

double idol::Optimizers::HiGHS::get_absolute_gap() const {
    return 0;
}

unsigned int idol::Optimizers::HiGHS::get_n_solutions() const {
    const auto status = get_status();
    return status == Optimal || status == Feasible;
}

unsigned int idol::Optimizers::HiGHS::get_solution_index() const {
    return 0;
}

void idol::Optimizers::HiGHS::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds");
    }
}

#endif
