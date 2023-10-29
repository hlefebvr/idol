//
// Created by henri on 14/02/23.
//
#include <csignal>
#include "idol/optimizers/wrappers/HiGHS/Optimizers_HiGHS.h"
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

    if (m_model.getNumCol() == 0) {
        m_model.addCol(0., 0., 0., 0, nullptr, nullptr);
    }

}

void idol::Optimizers::HiGHS::hook_write(const std::string &t_name) {
    m_model.writeModel(t_name);
}

void idol::Optimizers::HiGHS::set_var_type(int t_index, int t_type) {

    if (m_continuous_relaxation || t_type == Continuous) {
        m_model.changeColIntegrality(t_index, HighsVarType::kContinuous);
    } else if (t_type == Integer || t_type == Binary) {
        m_model.changeColIntegrality(t_index, HighsVarType::kInteger);
    } else {
        throw std::runtime_error("Unknown variable type.");
    }

}

void idol::Optimizers::HiGHS::set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj) {

    const bool has_lb = !is_neg_inf(t_lb);
    const bool has_ub = !is_pos_inf(t_ub);

    // Set obj
    m_model.changeColCost(t_index, t_obj);

    set_var_type(t_index, t_type);

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

    const int index = (int) m_model.getNumCol();

    double lb = parent().get_var_lb(t_var);
    double ub = parent().get_var_ub(t_var);
    const auto& column = parent().get_var_column(t_var);
    const auto type = parent().get_var_type(t_var);

    lb = is_neg_inf(lb) ? -m_model.getInfinity() : lb;
    ub = is_pos_inf(ub) ? m_model.getInfinity() : ub;

    if (t_add_column) {

        const auto& column_linear = column.linear();

        unsigned int n_coefficients = column_linear.size();
        auto* ctr_indices = new int[n_coefficients];
        auto* ctr_coefficients = new double[n_coefficients];

        unsigned int i = 0;
        for (const auto& [ctr, constant] : column_linear) {
            int ctr_index = lazy(ctr).impl();
            ctr_indices[i] = ctr_index;
            ctr_coefficients[i] = as_numeric(constant);
            ++i;
        }

        m_model.addCol(as_numeric(column.obj()),
                       lb,
                       ub,
                       (int) n_coefficients,
                       ctr_indices,
                       ctr_coefficients);

    } else {
        m_model.addCol(as_numeric(column.obj()),
                       lb,
                       ub,
                       0,
                       nullptr,
                       nullptr);
    }

    set_var_type(index, type);

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

    const int index = (int) m_model.getNumRow();

    const auto& row = parent().get_ctr_row(t_ctr);
    const auto& row_linear = row.linear();
    const double rhs = as_numeric(row.rhs());
    const auto type = parent().get_ctr_type(t_ctr);

    unsigned int n_coefficients = row_linear.size();
    auto* var_indices = new int[n_coefficients];
    auto* var_coefficients = new double[n_coefficients];

    unsigned int i = 0;
    for (const auto& [var, constant] : row.linear()) {
        int ctr_index = lazy(var).impl();
        var_indices[i] = ctr_index;
        var_coefficients[i] = as_numeric(constant);
        ++i;
    }

    switch (type) {
        case LessOrEqual:
            m_model.addRow(-m_model.getInfinity(),
                           rhs,
                           (int) n_coefficients,
                           var_indices,
                           var_coefficients);
            break;
        case GreaterOrEqual:
            m_model.addRow(rhs,
                           m_model.getInfinity(),
                           (int) n_coefficients,
                           var_indices,
                           var_coefficients);
            break;
        case Equal:
            m_model.addRow(rhs,
                           rhs,
                           (int) n_coefficients,
                           var_indices,
                           var_coefficients);
            break;
        default:
            throw std::runtime_error("Unknown constraint type.");
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

    m_model.deleteCols(index, index);

    for (auto& lazy_var : lazy_vars()) {
        if (lazy_var.has_impl() && lazy_var.impl() > index) {
            lazy_var.impl() -= 1;
        }
    }

}

void idol::Optimizers::HiGHS::hook_remove(const Ctr &t_ctr) {

    const int index = lazy(t_ctr).impl();

    m_model.deleteRows(index, index);

    for (auto& lazy_ctr : lazy_ctrs()) {
        if (lazy_ctr.has_impl() && lazy_ctr.impl() > index) {
            lazy_ctr.impl() -= 1;
        }
    }

}

void idol::Optimizers::HiGHS::hook_optimize() {

    delete[] m_extreme_ray;
    m_extreme_ray = nullptr;

    delete[] m_farkas_certificate;
    m_farkas_certificate = nullptr;

    m_model.run();

    analyze_status();

    if (m_solution_status == Fail) {
        m_model.clearSolver();
        m_model.run();
        analyze_status();
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

    if (get_param_infeasible_or_unbounded_info() && m_solution_status == Infeasible && get_remaining_time() > 0) {

        bool has_dual_ray;
        m_farkas_certificate = new double[m_model.getNumRow()];
        m_model.getDualRay(has_dual_ray, m_farkas_certificate);

        if (!has_dual_ray) {
            run_without_presolve();
            m_model.getDualRay(has_dual_ray, m_farkas_certificate);
            if (!has_dual_ray) {
                throw Exception("Cannot save Farkas certificate.");
            }
        }

    }

}

void idol::Optimizers::HiGHS::analyze_status() {

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
        case HighsModelStatus::kTimeLimit:
            m_solution_status = m_model.getSolution().value_valid ? Feasible : Infeasible;
            m_solution_reason = TimeLimit;
            break;
        case HighsModelStatus::kObjectiveBound:
        case HighsModelStatus::kObjectiveTarget:
        case HighsModelStatus::kIterationLimit:
        case HighsModelStatus::kSolutionLimit:
        case HighsModelStatus::kInterrupt:
            throw Exception("Unhandled status: " + std::to_string((int) status));
    }

}

void idol::Optimizers::HiGHS::run_without_presolve() {
    std::string old_presolve_setting;
    m_model.getOptionValue("presolve", old_presolve_setting);
    if (old_presolve_setting == "off") {
        return;
    }
    m_model.setOptionValue("presolve", "off");
    m_model.run();
    m_model.setOptionValue("presolve", old_presolve_setting);
}

void idol::Optimizers::HiGHS::set_param_time_limit(double t_time_limit) {
    m_model.setOptionValue("time_limit", m_model.getRunTime() + t_time_limit);
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
    m_model.setOptionValue("presolve", t_value ? "on" : "off");
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
    /*
    if (!m_model.getSolution().value_valid) {
        throw Exception("Cannot access primal values");
    }
     */
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

void idol::Optimizers::HiGHS::set_param_log_level(idol::LogLevel t_log_level) {

    if (t_log_level > Mute) {
        m_model.setOptionValue("output_flag", true);
    } else {
        m_model.setOptionValue("output_flag", false);
    }

    Optimizer::set_param_log_level(t_log_level);
}

idol::Optimizers::HiGHS::~HiGHS() {
    delete[] m_farkas_certificate;
    delete[] m_extreme_ray;
}

#endif
