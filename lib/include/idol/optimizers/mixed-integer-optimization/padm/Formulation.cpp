//
// Created by henri on 18.09.24.
//

#include "Formulation.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"

#include <utility>

idol::ADM::Formulation::Formulation(const Model& t_src_model,
                                    Annotation<Var, unsigned int> t_decomposition,
                                    std::optional<Annotation<Ctr, bool>> t_penalized_constraints)
                                                     : m_decomposition(std::move(t_decomposition)),
                                                       m_penalized_constraints(std::move(t_penalized_constraints)) {

    const auto n_sub_problems = compute_n_sub_problems(t_src_model);

    initialize_sub_problems(t_src_model, n_sub_problems);
    initialize_patterns(t_src_model, n_sub_problems);
    initialize_slacks(t_src_model, n_sub_problems);

    dispatch_vars(t_src_model);
    dispatch_ctrs(t_src_model);
    dispatch_obj(t_src_model);

}

unsigned int idol::ADM::Formulation::compute_n_sub_problems(const idol::Model &t_src_model) const {
    unsigned int result = 0;
    for (const auto& var : t_src_model.vars()) {
        result = std::max(result, var.get(m_decomposition));
    }
    return result + 1;
}

void idol::ADM::Formulation::initialize_sub_problems(const idol::Model &t_src_model,
                                                     unsigned int n_sub_problems) {

    auto& env = t_src_model.env();
    m_sub_problems.reserve(n_sub_problems);

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        m_sub_problems.emplace_back(env);
    }

}

void idol::ADM::Formulation::initialize_patterns(const idol::Model &t_src_model,
                                                 unsigned int n_sub_problems) {

    m_objective_patterns.resize(n_sub_problems);
    m_constraint_patterns.resize(n_sub_problems);

}

void idol::ADM::Formulation::initialize_slacks(const idol::Model &t_src_model,
                                               unsigned int n_sub_problems) {
    m_l1_vars.resize(n_sub_problems);
}

void idol::ADM::Formulation::dispatch_vars(const idol::Model &t_src_model) {

    for (const auto& var : t_src_model.vars()) {

        const unsigned int sub_problem_id = var.get(m_decomposition);
        const auto lb = t_src_model.get_var_lb(var);
        const auto ub = t_src_model.get_var_ub(var);
        const auto type = t_src_model.get_var_type(var);

        m_sub_problems[sub_problem_id].add(var, TempVar(lb, ub, type, Column()));

    }

}

void idol::ADM::Formulation::dispatch_ctrs(const idol::Model &t_src_model) {

    const auto n_sub_problems = m_sub_problems.size();

    for (const auto& ctr : t_src_model.ctrs()) {
        for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
            dispatch_ctr(t_src_model, ctr, i);
        }
    }

}

void idol::ADM::Formulation::dispatch_ctr(const idol::Model &t_src_model, const idol::Ctr &t_ctr, unsigned int t_sub_problem_id) {

    const auto& row = t_src_model.get_ctr_row(t_ctr);
    const auto type = t_src_model.get_ctr_type(t_ctr);

    auto [pattern, is_pure] = dispatch(t_src_model, row.linear(), row.quadratic(),  t_sub_problem_id);
    pattern.constant() -= row.rhs();

    if (pattern.linear().empty() && pattern.quadratic().empty()) {
        return;
    }

    if (m_penalized_constraints && t_ctr.get(*m_penalized_constraints)) {
        auto& model = m_sub_problems[t_sub_problem_id];

        const auto add_l1_var = [&](double t_coefficient) {
            auto var = model.add_var(0, Inf, Continuous, Column(), "l1_norm_" + std::to_string(t_coefficient < 0) + "_" + t_ctr.name());
            pattern.linear() += t_coefficient * var;
            m_l1_vars[t_sub_problem_id].emplace_back(var);
        };

        switch (type) {
            case Equal:
                add_l1_var(+1);
                add_l1_var(-1);
                break;
            case LessOrEqual:
                add_l1_var(-1);
                break;
            case GreaterOrEqual:
                add_l1_var(+1);
                break;
        }
    }

    if (is_pure) {
        m_sub_problems[t_sub_problem_id].add(t_ctr, TempCtr(Row(std::move(pattern), 0), type));
    } else {
        m_sub_problems[t_sub_problem_id].add(t_ctr, TempCtr(Row(), type));
        m_constraint_patterns[t_sub_problem_id].emplace_back(t_ctr, std::move(pattern));
    }

}

void
idol::ADM::Formulation::dispatch_obj(const Model &t_src_model) {

    const unsigned int n_sub_problems = m_sub_problems.size();

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
        dispatch_obj(t_src_model, i);
    }

}

std::pair<idol::Expr<idol::Var, idol::Var>, bool> idol::ADM::Formulation::dispatch(const idol::Model &t_src_model,
                                                                                   const idol::LinExpr<idol::Var> &t_lin_expr,
                                                                                   const idol::QuadExpr<idol::Var, idol::Var> &t_quad_expr,
                                                                                   unsigned int t_sub_problem_id) {

    bool is_pure = true; // true if the row only has variables from the same sub-problem

    Expr pattern;

    for (const auto& [var, coefficient] : t_lin_expr) {

        const unsigned int var_sub_problem_id = var.get(m_decomposition);

        if (var_sub_problem_id != t_sub_problem_id) {
            is_pure = false;
            pattern.constant() += coefficient.as_numerical() * !var;
            continue;
        }

        pattern.linear() += coefficient * var;
    }

    for (const auto& [var1, var2, constant] : t_quad_expr) {

        const unsigned int var1_sub_problem_id = var1.get(m_decomposition);
        const unsigned int var2_sub_problem_id = var2.get(m_decomposition);

        if (var1_sub_problem_id != t_sub_problem_id && var2_sub_problem_id != t_sub_problem_id) {
            is_pure = false;
            pattern.constant() += constant.as_numerical() * (!var1 * !var2);
            continue;
        }

        if (var1_sub_problem_id != t_sub_problem_id) {
            is_pure = false;
            pattern.linear() += constant.as_numerical() * !var1 * var2;
            continue;
        }

        if (var2_sub_problem_id != t_sub_problem_id) {
            is_pure = false;
            pattern.linear() += constant.as_numerical() * !var2 * var1;
            continue;
        }

        pattern.quadratic() += constant * var1 * var2;

    }

    return {
        std::move(pattern),
        is_pure
    };
}

void
idol::ADM::Formulation::dispatch_obj(const Model &t_src_model, unsigned int t_sub_problem_id) {

    const auto& obj = t_src_model.get_obj_expr();
    auto [pattern, is_pure] = dispatch(t_src_model, obj.linear(), obj.quadratic(), t_sub_problem_id);
    pattern += obj.constant();

    if (pattern.linear().empty() && pattern.quadratic().empty()) {
        return;
    }

    if (is_pure) {
        m_sub_problems[t_sub_problem_id].set_obj_expr(pattern);
        return;
    }

    m_objective_patterns[t_sub_problem_id] = std::move(pattern);

}

void idol::ADM::Formulation::fix_sub_problem(unsigned int t_sub_problem_id,
                                             const std::vector<Solution::Primal> &t_primals) {

    // Constraints
    for (const auto& [ctr, pattern] : m_constraint_patterns[t_sub_problem_id]) {

        Expr lhs = fix(pattern.constant(), t_primals);

        for (const auto& [var, coefficient] : pattern.linear()) {
            lhs += fix(coefficient, t_primals) * var;
        }

        for (const auto& [var1, var2, coefficient] : pattern.quadratic()) {
            lhs += fix(coefficient, t_primals) * var1 * var2;
        }

        m_sub_problems[t_sub_problem_id].set_ctr_row(ctr, Row(std::move(lhs), 0));
    }

    // Objective
    if (m_objective_patterns[t_sub_problem_id]) {
        const auto& obj_pattern = *m_objective_patterns[t_sub_problem_id];
        Expr obj = fix(obj_pattern.constant(), t_primals);

        for (const auto& [var, coefficient] : obj_pattern.linear()) {
            obj += fix(coefficient, t_primals) * var;
        }

        for (const auto& [var1, var2, coefficient] : obj_pattern.quadratic()) {
            obj += fix(coefficient, t_primals) * var1 * var2;
        }

        m_sub_problems[t_sub_problem_id].set_obj_expr(std::move(obj));

    }

}

double idol::ADM::Formulation::fix(const idol::Constant &t_constant,
                                   const std::vector<Solution::Primal> &t_primals) {
    double result = t_constant.numerical();

    for (const auto& [param, coefficient] : t_constant.linear()) {
        const auto& var = param.as<Var>();
        const auto var_sub_problem_id = var.get(m_decomposition);
        const auto& solution = t_primals[var_sub_problem_id];
        result += coefficient * solution.get(var);
    }

    for (const auto& [params, coefficient] : t_constant.quadratic()) {
        const auto& var1 = params.first.as<Var>();
        const auto& var2 = params.second.as<Var>();
        const auto var1_sub_problem_id = var1.get(m_decomposition);
        const auto var2_sub_problem_id = var2.get(m_decomposition);
        const auto& solution1 = t_primals[var1_sub_problem_id];
        const auto& solution2 = t_primals[var2_sub_problem_id];
        result += coefficient * solution1.get(var1) * solution2.get(var2);
    }

    return result;
}

idol::Model &idol::ADM::Formulation::sub_problem(const idol::Var &t_var) {
    return m_sub_problems[t_var.get(m_decomposition)];
}

const idol::Model &idol::ADM::Formulation::sub_problem(const idol::Var &t_var) const {
    return m_sub_problems[t_var.get(m_decomposition)];
}

void
idol::ADM::Formulation::update_penalty_parameters(const std::vector<Solution::Primal> &t_primals,
                                                  PenaltyUpdate& t_penalty_update) {

    for (unsigned int i = 0, n_sub_problems = m_sub_problems.size() ; i < n_sub_problems ; ++i) {
        auto& model = m_sub_problems[i];
        for (const auto& var : m_l1_vars[i]) {

            double current_penalty = model.get_var_column(var).obj().as_numerical();

            if (current_penalty < 1e-3) {
                model.set_var_obj(var, 1);
                continue;
            }

            if (t_primals[i].get(var) > 1e-4) {
                model.set_var_obj(var, t_penalty_update(current_penalty));
            }

        }
    }

}
