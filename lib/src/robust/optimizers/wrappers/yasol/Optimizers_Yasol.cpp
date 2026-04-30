//
// Created by Henri on 29/04/2026.
//
#include "idol/robust/optimizers/wrappers/yasol/Optimizers_Yasol.h"

#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/robust/optimizers/wrappers/yasol/QpIdolExternSolver.h"

#ifdef IDOL_USE_YASOL
#include "QBPSolver.h"
#include "Datastructures/qlp/Qlp.hpp"
#include  "yInterface.h"
#include  "ExternSolvers/QpExtSolCBC.hpp"
#endif

#define THROW_NOT_IMPLEMENTED throw std::runtime_error(std::string(__FUNCTION__) + " is not implemented.\n" + std::string(__FILE__) + ":" + std::to_string(__LINE__));

struct YasolAttributes {
#ifdef IDOL_USE_YASOL
    data::Qlp qlp_model;
    yInterface yasol;

    std::vector<data::QpVar*> model_variable_in_qlp_model;
    std::vector<data::QpVar*> uncertainty_set_variable_in_qlp_model;
#endif
};

idol::Optimizers::Robust::Yasol::Yasol(const Model& t_parent, const idol::Robust::Description& t_robust_description, const idol::Bilevel::Description* t_bilevel_description) :
    Optimizer(t_parent),
    m_robust_description(t_robust_description),
    m_bilevel_description(t_bilevel_description) {

#ifdef IDOL_USE_YASOL

#else
    throw Exception("idol was not linked to Yasol.");
#endif

}

idol::Optimizers::Robust::Yasol::~Yasol() {
    if (m_impl) {
        delete static_cast<YasolAttributes*>(m_impl);
        m_impl = nullptr;
    }
}

std::string idol::Optimizers::Robust::Yasol::name() const {
    return "yasol-robust";
}

#ifdef IDOL_USE_YASOL
data::QpVar::NumberSystem to_yasol(idol::VarType t_var_type) {

    switch (t_var_type) {
        case idol::Continuous: return data::QpVar::real;
        case idol::Integer: return data::QpVar::generals;
        case idol::Binary: return data::QpVar::binaries;
        default: throw idol::Exception("Unexpected variable type.");
    }

}

data::QpRhs::RatioSign to_yasol(idol::CtrType t_ctr_type) {
    switch (t_ctr_type) {
        case idol::LessOrEqual: return data::QpRhs::smallerThanOrEqual;
        case idol::GreaterOrEqual: return data::QpRhs::greaterThanOrEqual;
        case idol::Equal: return data::QpRhs::equal;
        default: throw idol::Exception("Unexpected constraint type.");
    }
}
#endif

void idol::Optimizers::Robust::Yasol::build() {
#ifdef IDOL_USE_YASOL
    delete static_cast<YasolAttributes*>(m_impl);

    auto* impl = new YasolAttributes;
    m_impl = impl;

    const auto& model = this->parent();
    impl->model_variable_in_qlp_model.resize(model.vars().size(), nullptr);

    const auto& uncertainty_set = m_robust_description.uncertainty_set();
    impl->uncertainty_set_variable_in_qlp_model.resize(uncertainty_set.vars().size(), nullptr);

    // First-stage variables
    for (const auto& var : model.vars()) {

        if (m_bilevel_description && m_bilevel_description->is_lower(var)) {
            continue;
        }

        const auto lb = model.get_var_lb(var);
        const auto ub = model.get_var_ub(var);
        const auto obj = model.get_var_obj(var);
        const auto type = model.get_var_type(var);
        const auto index = model.get_var_index(var);

        auto& result = impl->qlp_model.createVariable( var.name(), to_yasol(type), lb, ub, data::QpVar::exists, obj);

        impl->model_variable_in_qlp_model[index] = &result;

    }

    // First-stage constraints
    for (const auto& ctr : model.ctrs()) {

        if (m_bilevel_description && m_bilevel_description->is_lower(ctr)) {
            continue;
        }

        const auto type = model.get_ctr_type(ctr);
        const auto rhs = model.get_ctr_rhs(ctr);
        const auto& row = model.get_ctr_row(ctr);

        auto& qlp_ctr = impl->qlp_model.createRhsConstraint(to_yasol(type), rhs, data::Constraint::EXISTENTIAL);

        for (const auto& [var, coeff] : row) {
            auto& qlp_var = *impl->model_variable_in_qlp_model[model.get_var_index(var)];
            impl->qlp_model.addConstraintElement(qlp_ctr, qlp_var, coeff);
        }

    }

    // Uncertainty variables
    for (const auto& var : uncertainty_set.vars()) {

        if (model.has(var)) {
            continue;
        }

        const auto lb = uncertainty_set.get_var_lb(var);
        const auto ub = uncertainty_set.get_var_ub(var);
        const auto obj = uncertainty_set.get_var_obj(var);
        const auto type = uncertainty_set.get_var_type(var);
        const auto index = uncertainty_set.get_var_index(var);

        auto& result = impl->qlp_model.createVariable( var.name(), to_yasol(type), lb, ub, data::QpVar::all, obj);

        impl->uncertainty_set_variable_in_qlp_model[index] = &result;

    }

    // Uncertainty constraint
    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto type = uncertainty_set.get_ctr_type(ctr);
        const auto rhs = uncertainty_set.get_ctr_rhs(ctr);
        const auto& row = uncertainty_set.get_ctr_row(ctr);

        auto& qlp_ctr = impl->qlp_model.createRhsConstraint(to_yasol(type), rhs, data::Constraint::UNIVERSAL);

        for (const auto& [var, coeff] : row) {
            if (model.has(var)) {
                auto& qlp_var = *impl->model_variable_in_qlp_model[model.get_var_index(var)];
                impl->qlp_model.addConstraintElement(qlp_ctr, qlp_var, coeff);
            } else {
                auto& qlp_var = *impl->uncertainty_set_variable_in_qlp_model[uncertainty_set.get_var_index(var)];
                impl->qlp_model.addConstraintElement(qlp_ctr, qlp_var, coeff);
            }
        }

    }

    // Second-stage variables
    if (m_bilevel_description) {
        for (const auto& var : model.vars()) {

            if (m_bilevel_description->is_upper(var)) {
                continue;
            }

            const auto lb = model.get_var_lb(var);
            const auto ub = model.get_var_ub(var);
            const auto obj = model.get_var_obj(var);
            const auto type = model.get_var_type(var);
            const auto index = model.get_var_index(var);

            auto& result = impl->qlp_model.createVariable( var.name(), to_yasol(type), lb, ub, data::QpVar::exists, obj);

            impl->model_variable_in_qlp_model[index] = &result;

        }

        for (const auto& ctr : model.ctrs()) {

            if (m_bilevel_description->is_upper(ctr)) {
                continue;
            }

            const auto type = model.get_ctr_type(ctr);
            const auto rhs = model.get_ctr_rhs(ctr);
            const auto& row = model.get_ctr_row(ctr);

            auto& qlp_ctr = impl->qlp_model.createRhsConstraint(to_yasol(type), rhs, data::Constraint::EXISTENTIAL);

            for (const auto& [var, coeff] : row) {
                auto& qlp_var = *impl->model_variable_in_qlp_model[model.get_var_index(var)];
                impl->qlp_model.addConstraintElement(qlp_ctr, qlp_var, coeff);
            }

            // Right-hand side uncertainty
            for (const auto& [unc_param, coeff] : m_robust_description.uncertain_rhs(ctr)) {
                auto& qlp_unc_var = *impl->uncertainty_set_variable_in_qlp_model[uncertainty_set.get_var_index(unc_param)];
                impl->qlp_model.addConstraintElement(qlp_ctr, qlp_unc_var, -coeff);
            }

            // Objective uncertainty
            if (m_robust_description.uncertain_obj().size() > 0) {
                throw Exception("Objective uncertainty is not supported by Yasol.");
            }

            // Matrix uncertainty
            for (const auto& [var, expr] : m_robust_description.uncertain_mat_coeffs(ctr)) {
                for (const auto& [unc_var, coeff] : expr) {
                    const auto var_type = model.get_var_type(var);
                    const auto unc_var_type = uncertainty_set.get_var_type(unc_var);

                    if (var_type != Binary || unc_var_type != Binary) {
                        throw Exception("Only products of binary variables are allowed in matrix uncertainty when using Yasol.");
                    }

                    auto& qlp_product = impl->qlp_model.createVariable("prod_" + var.name() + "_" + unc_var.name(), data::QpVar::real, 0, 1, data::QpVar::exists, 0);
                    auto& qlp_var = *impl->model_variable_in_qlp_model[model.get_var_index(var)];
                    auto& qlp_unc_var = *impl->uncertainty_set_variable_in_qlp_model[uncertainty_set.get_var_index(unc_var)];

                    // (1) w ≤ x  →  w - x ≤ 0
                    auto& c1 = impl->qlp_model.createRhsConstraint(data::QpRhs::smallerThanOrEqual, 0, data::Constraint::EXISTENTIAL);
                    impl->qlp_model.addConstraintElement(c1, qlp_product, 1.0);
                    impl->qlp_model.addConstraintElement(c1, qlp_var, -1.0);

                    // (2) w ≤ u  →  w - u ≤ 0
                    auto& c2 = impl->qlp_model.createRhsConstraint(data::QpRhs::smallerThanOrEqual, 0, data::Constraint::EXISTENTIAL);
                    impl->qlp_model.addConstraintElement(c2, qlp_product, 1.0);
                    impl->qlp_model.addConstraintElement(c2, qlp_unc_var, -1.0);

                    // (3) w ≥ x + u - 1  →  w - x - u ≥ -1
                    auto& c3 = impl->qlp_model.createRhsConstraint(data::QpRhs::greaterThanOrEqual, -1, data::Constraint::EXISTENTIAL);
                    impl->qlp_model.addConstraintElement(c3, qlp_product, 1.0);
                    impl->qlp_model.addConstraintElement(c3, qlp_var, -1.0);
                    impl->qlp_model.addConstraintElement(c3, qlp_unc_var, -1.0);

                    impl->qlp_model.addConstraintElement(qlp_ctr, qlp_product, coeff);

                }
            }
        }

    }
#endif
}

void idol::Optimizers::Robust::Yasol::add(const Var& t_var) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::add(const Ctr& t_ctr) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::add(const QCtr& t_ctr) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::hook_optimize() {
#ifdef IDOL_USE_YASOL
    delete static_cast<YasolAttributes*>(m_impl);
    m_impl = nullptr;

    if (!m_impl) {
        build();
    }

    auto* impl = static_cast<YasolAttributes*>(m_impl);

    impl->yasol.yInit(impl->qlp_model);

    if (get_param_logs()) {
        impl->yasol.setParam("showInfo", true);
        impl->yasol.setParam("showWarning", false);
        impl->yasol.setParam("showError", true);
    } else {
        impl->yasol.supressOutput();
    }

    const time_t time_limit = (time_t) std::min<double>(get_param_time_limit(), (double) (std::numeric_limits<time_t>::max() - std::time(nullptr) - 10000));
    impl->yasol.setTimelimit(time_limit);

    impl->yasol.solve();

#endif
}

void idol::Optimizers::Robust::Yasol::set_solution_index(unsigned t_index) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::remove(const Var& t_var) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::remove(const Ctr& t_ctr) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::remove(const QCtr& t_ctr) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update() {

}

void idol::Optimizers::Robust::Yasol::write(const std::string& t_name) {

    if (!m_impl) {
        build();
    }

    std::ofstream file(t_name);

    if (!file.is_open()) {
        throw Exception("Could not create file.");
    }

    file << static_cast<YasolAttributes*>(m_impl)->qlp_model.toQlpFileString(false);

    file.close();
}

void idol::Optimizers::Robust::Yasol::update_obj_sense() {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_obj() {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_rhs() {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_obj_constant() {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_mat_coeff(const Ctr& t_ctr, const Var& t_var) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_ctr_type(const Ctr& t_ctr) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_ctr_rhs(const Ctr& t_ctr) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_var_type(const Var& t_var) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_var_lb(const Var& t_var) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_var_ub(const Var& t_var) {
    THROW_NOT_IMPLEMENTED
}

void idol::Optimizers::Robust::Yasol::update_var_obj(const Var& t_var) {
    THROW_NOT_IMPLEMENTED
}

idol::SolutionStatus idol::Optimizers::Robust::Yasol::get_status() const {
#ifdef IDOL_USE_YASOL
    if (!m_impl) {
        return Loaded;
    }

    auto status = static_cast<YasolAttributes*>(m_impl)->yasol.getStatus();

    switch (status) {
        case YASOL_FEASIBLE: [[fallthrough]];
        case YASOL_INCUMBENT:
            return Feasible;
        case YASOL_UNSAT:
            return Infeasible;
        case YASOL_ERROR:
            return Fail;
        case YASOL_OPTIMAL:
            return Optimal;
        case YASOL_UNKNOWN:
            return Loaded;
        default: throw Exception("Unexpected Yasol status " + std::to_string(status) + ".");
    }
#endif
}

idol::SolutionReason idol::Optimizers::Robust::Yasol::get_reason() const {
#ifdef IDOL_USE_YASOL
    if (!m_impl) {
        return NotSpecified;
    }

    auto status = static_cast<YasolAttributes*>(m_impl)->yasol.getStatus();
    if (status == YASOL_INCUMBENT || status == YASOL_OPTIMAL || status == YASOL_FEASIBLE) {
        return Proved;
    }
    return NotSpecified;
#endif
}

double idol::Optimizers::Robust::Yasol::get_best_obj() const {
#ifdef IDOL_USE_YASOL
    if (!m_impl) {
        return Inf;
    }
    return static_cast<YasolAttributes*>(m_impl)->yasol.getResult();
#endif
}

double idol::Optimizers::Robust::Yasol::get_best_bound() const {
#ifdef IDOL_USE_YASOL
    if (!m_impl) {
        return -Inf;
    }
    return static_cast<YasolAttributes*>(m_impl)->yasol.getDual();
#endif
}

double idol::Optimizers::Robust::Yasol::get_var_primal(const Var& t_var) const {
    THROW_NOT_IMPLEMENTED
}

double idol::Optimizers::Robust::Yasol::get_var_reduced_cost(const Var& t_var) const {
    THROW_NOT_IMPLEMENTED
}

double idol::Optimizers::Robust::Yasol::get_var_ray(const Var& t_var) const {
    THROW_NOT_IMPLEMENTED
}

double idol::Optimizers::Robust::Yasol::get_ctr_dual(const Ctr& t_ctr) const {
    THROW_NOT_IMPLEMENTED
}

double idol::Optimizers::Robust::Yasol::get_ctr_farkas(const Ctr& t_ctr) const {
    THROW_NOT_IMPLEMENTED
}

double idol::Optimizers::Robust::Yasol::get_relative_gap() const {
    THROW_NOT_IMPLEMENTED
}

double idol::Optimizers::Robust::Yasol::get_absolute_gap() const {
    THROW_NOT_IMPLEMENTED
}

unsigned idol::Optimizers::Robust::Yasol::get_n_solutions() const {
    THROW_NOT_IMPLEMENTED
}

unsigned idol::Optimizers::Robust::Yasol::get_solution_index() const {
    THROW_NOT_IMPLEMENTED
}
