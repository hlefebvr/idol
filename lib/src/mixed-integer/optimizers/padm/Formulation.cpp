//
// Created by henri on 18.09.24.
//

#include "idol/mixed-integer/optimizers/padm/Formulation.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/padm/PenaltyUpdates.h"
#include "idol/mixed-integer/modeling/variables/TempVar.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"

#include <utility>
#include <cassert>

idol::ADM::Formulation::Formulation(const Model& t_src_model,
                                    Annotation<unsigned int> t_decomposition,
                                    std::optional<Annotation<double>> t_penalized_constraints,
                                    double t_rescaling)
                                 : m_decomposition(std::move(t_decomposition)),
                                   m_initial_penalty_parameters(std::move(t_penalized_constraints)),
                                   m_rescaling_threshold(t_rescaling) {

    const auto n_sub_problems = compute_n_sub_problems(t_src_model);

    initialize_sub_problems(t_src_model, n_sub_problems);

    dispatch_vars(t_src_model);
    dispatch_ctrs(t_src_model);
    dispatch_qctrs(t_src_model);
    dispatch_obj(t_src_model);

}

unsigned int idol::ADM::Formulation::compute_n_sub_problems(const idol::Model &t_src_model) const {

    unsigned int result = 0;

    for (const auto& var : t_src_model.vars()) {

        const unsigned int sub_problem_id = var.get(m_decomposition);

        if (sub_problem_id == (unsigned int) -1) {
            continue;
        }

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

void idol::ADM::Formulation::dispatch_vars(const idol::Model &t_src_model) {

    const unsigned int n_sub_problems = m_sub_problems.size();

    for (const auto& var : t_src_model.vars()) {

        const unsigned int sub_problem_id = var.get(m_decomposition);
        const auto lb = t_src_model.get_var_lb(var);
        const auto ub = t_src_model.get_var_ub(var);
        const auto type = t_src_model.get_var_type(var);

        if (sub_problem_id == -1) {

            for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
                m_sub_problems[i].model.add(var, TempVar(lb, ub, type, 0.,LinExpr<Ctr>()));
            }

            continue;
        }

        m_sub_problems[sub_problem_id].model.add(var, TempVar(lb, ub, type, 0., LinExpr<Ctr>()));

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

    auto [lhs, rhs] = dispatch(row, t_sub_problem_id);
    rhs += t_src_model.get_ctr_rhs(t_ctr);

    if (lhs.empty() && !row.empty()) {
        return;
    }

    if (m_initial_penalty_parameters && t_ctr.get(*m_initial_penalty_parameters) > 1e-4) {
        lhs += add_l1_vars(t_ctr, type, t_sub_problem_id);
    }

    auto& sub_problem = m_sub_problems[t_sub_problem_id];
    if (rhs.linear().empty()) {
        sub_problem.model.add(t_ctr, TempCtr(std::move(lhs), type, rhs.constant()));
    } else {
        sub_problem.model.add(t_ctr, TempCtr(std::move(lhs), type, 0));
        sub_problem.rhs_fixations.emplace_back(t_ctr, std::move(rhs));
    }

}

void
idol::ADM::Formulation::dispatch_obj(const Model &t_src_model) {
    for (unsigned int i = 0 ; i < m_sub_problems.size() ; ++i) {
        dispatch_obj(t_src_model, i);
    }
}

void
idol::ADM::Formulation::dispatch_obj(const Model &t_src_model, unsigned int t_sub_problem_id) {
    const auto& objective = t_src_model.get_obj_expr();
    auto& obj_fixation = m_sub_problems[t_sub_problem_id].obj_fixation;

    // Linear part
    for (const auto& [var, coeff] : objective.affine().linear()) {
        if (var.get(m_decomposition) == t_sub_problem_id || var.get(m_decomposition) == -1) {
            obj_fixation.affine().linear().set(var, coeff);
        } else {
            obj_fixation.affine().constant() += coeff * var;
        }
    }

    // Quadratic part
    for (const auto& [pair, coeff] : objective) {

        const bool first_in_sub_problem = pair.first.get(m_decomposition) == t_sub_problem_id || pair.first.get(m_decomposition) == -1;
        const bool second_in_sub_problem = pair.second.get(m_decomposition) == t_sub_problem_id || pair.second.get(m_decomposition) == -1;

        if (first_in_sub_problem && second_in_sub_problem) {
            obj_fixation.set(pair, coeff);
            continue;
        }

        if (first_in_sub_problem) {
            auto current_coeff = obj_fixation.affine().linear().get(pair.first);
            obj_fixation.affine().linear().set(pair.first, std::move(current_coeff) + coeff * pair.second);
            continue;
        }

        if (second_in_sub_problem) {
            auto current_coeff = obj_fixation.affine().linear().get(pair.second);
            obj_fixation.affine().linear().set(pair.second, std::move(current_coeff) + coeff * pair.first);
            continue;
        }

        // extra term would be fixed in objective function anyway

    }

}

std::pair<idol::LinExpr<idol::Var>, idol::AffExpr<idol::Var>>
idol::ADM::Formulation::dispatch(const idol::LinExpr<idol::Var> &t_expr, unsigned int t_sub_problem_id) {

    LinExpr<Var> lhs;
    AffExpr<Var> rhs;

    for (const auto& [var, coeff] : t_expr) {
        const unsigned int var_sub_problem_id = var.get(m_decomposition);
        if (var_sub_problem_id == t_sub_problem_id || var_sub_problem_id == -1) {
            lhs += coeff * var;
        } else {
            rhs -= coeff * var;
        }
    }

    return std::make_pair(std::move(lhs), std::move(rhs));
}

bool
idol::ADM::Formulation::update_penalty_parameters(const std::vector<PrimalPoint> &t_primals,
                                                  PenaltyUpdate& t_penalty_update) {

    const unsigned int n_sub_problems = m_sub_problems.size();

    std::list<CurrentPenalty> current_penalties;

    for (auto &[ctr_id, var] : m_l1_epigraph_vars) {

        unsigned int argmax = -1;
        double max = 0.;
        double penalty = 0;

        for (unsigned int i = 0; i < n_sub_problems; ++i) {

            if (const double val = t_primals[i].get(var); val > max) {
                max = val;
                argmax = i;
                penalty = m_sub_problems[i].model.get_var_obj(var);
            }

        }

        if (argmax == -1 || max <= 1e-4) {
            continue;
        }

        current_penalties.emplace_back(var, max, penalty);

    }

    t_penalty_update(current_penalties);

    bool has_rescaled = false;
    if (m_rescaling_threshold > 0) {
        has_rescaled = rescale_penalty_parameters(current_penalties);
    }

    for (const auto& [var, violation, penalty] : current_penalties) {
        set_penalty_in_all_sub_problems(var, penalty);
    }

    return has_rescaled;
}

void idol::ADM::Formulation::set_penalty_in_all_sub_problems(const Var &t_var, double t_value) {

    for (auto& sub_problem : m_sub_problems) {
        if (sub_problem.model.has(t_var)) {
            sub_problem.model.set_var_obj(t_var, t_value);
        }
    }

}

void idol::ADM::Formulation::initialize_penalty_parameters(bool t_use_inverse_penalties) {

    for (auto& sub_problem : m_sub_problems) {

        for (const auto& var : sub_problem.l1_epigraph_vars) {
            const double value = t_use_inverse_penalties ? 1. / var.get(*m_initial_penalty_parameters) : var.get(*m_initial_penalty_parameters);
            sub_problem.model.set_var_obj(var, value);
        }

    }

}

bool idol::ADM::Formulation::rescale_penalty_parameters(std::list<CurrentPenalty>& t_penalties) {

    double max = 0;
    for (const auto& penalty : t_penalties) {
        max = std::max(max, penalty.penalty);
    }

    if (max < m_rescaling_threshold) {
        return false;
    }

    const auto sigmoid = [&](double t_val) {
        const double alpha = max;
        constexpr double beta = 5;
        const double sigma = max;
        constexpr double omega = 5;
        return beta * (t_val - sigma) / (alpha + std::abs(t_val - sigma)) + omega;
    };

    max = 0;
    for (auto& penalty: t_penalties) {
        penalty.penalty = sigmoid(penalty.penalty);
        max = std::max(max, penalty.penalty);
    }

    return true;
}

unsigned int idol::ADM::Formulation::sub_problem_id(const idol::Var &t_var) const {
    return t_var.get(m_decomposition);
}

void idol::ADM::Formulation::update(unsigned int t_sub_problem_id, const std::vector<PrimalPoint> &t_primals) {

    auto& sub_problem = m_sub_problems[t_sub_problem_id];

    for (const auto& rhs_fixation : sub_problem.rhs_fixations) {
        sub_problem.model.set_ctr_rhs(rhs_fixation.ctr, evaluate(rhs_fixation.rhs_pattern, t_primals));
    }

    for (const auto& row_fixation : sub_problem.row_fixations) {

        if (std::holds_alternative<QCtr>(row_fixation.ctr)) {
            // TODO, here, we need to update the quadratic constraint
            auto qctr = std::get<QCtr>(row_fixation.ctr);
            auto eval = evaluate(row_fixation.row, t_primals);
            const auto type = sub_problem.model.get_qctr_type(qctr);
            sub_problem.model.remove(qctr);
            sub_problem.model.add(qctr, TempQCtr(std::move(eval), type));
            //throw Exception("Updating expression of quadratic constraints is not supported.");
        } else {

            auto ctr = std::get<Ctr>(row_fixation.ctr);

            auto eval = evaluate(row_fixation.row, t_primals);
            sub_problem.model.set_ctr_row(ctr, eval.affine().linear());
            sub_problem.model.set_ctr_rhs(ctr, -eval.affine().constant());

        }

    }

    std::cerr << "The objective function is systematically updated" << std::endl;

    std::list<std::pair<Var, double>> penalties;
    for (const auto& var : sub_problem.l1_epigraph_vars) {
        penalties.emplace_back(var, sub_problem.model.get_var_obj(var));
    }
    sub_problem.model.set_obj_expr(evaluate(sub_problem.obj_fixation, t_primals));
    for (const auto& [var, penalty] : penalties) {
        sub_problem.model.set_var_obj(var, penalty);
    }

}

idol::ADM::Formulation::SubProblem &idol::ADM::Formulation::sub_problem(const idol::Var &t_var) {
    return m_sub_problems[t_var.get(m_decomposition)];
}

const idol::ADM::Formulation::SubProblem &idol::ADM::Formulation::sub_problem(const idol::Var &t_var) const {
    return m_sub_problems[t_var.get(m_decomposition)];
}

void idol::ADM::Formulation::dispatch_qctrs(const idol::Model &t_src_model) {
    for (const auto& ctr : t_src_model.qctrs()) {
        for (unsigned int i = 0 ; i < m_sub_problems.size() ; ++i) {
            dispatch_qctr(t_src_model, ctr, i);
        }
    }
}

void idol::ADM::Formulation::dispatch_qctr(const idol::Model &t_src_model,
                                           const idol::QCtr &t_ctr,
                                           unsigned int t_sub_problem_id) {

    const auto& expr = t_src_model.get_qctr_expr(t_ctr);
    const auto type = t_src_model.get_qctr_type(t_ctr);

    // Linear part
    auto [lhs, rhs] = dispatch(expr.affine().linear(), t_sub_problem_id);
    rhs.constant() -= expr.affine().constant();

    if (m_initial_penalty_parameters && t_ctr.get(*m_initial_penalty_parameters) > 1e-4) {
        lhs += add_l1_vars(t_ctr, type, t_sub_problem_id);
    }

    QuadExpr<Var> rhs_quad(std::move(rhs));

    std::optional<QuadExpr<Var, QuadExpr<Var>>> full_row;

    // Quadratic part
    for (const auto& [pair, coeff] : expr) {

        const auto first_is_in_sub_problem = pair.first.get(m_decomposition) == t_sub_problem_id || pair.first.get(m_decomposition) == -1;
        const auto second_is_in_sub_problem = pair.second.get(m_decomposition) == t_sub_problem_id || pair.second.get(m_decomposition) == -1;

        if (!full_row) {

            if (!first_is_in_sub_problem && !second_is_in_sub_problem) { // here, we are still on RHS fixation
                rhs_quad -= coeff * pair.first * pair.second;
                continue;
            }

            // here, we have to switch to LHS fixation, hence, we put everything in the LHS
            // copy the lhs
            full_row = std::make_optional<QuadExpr<Var, QuadExpr<Var>>>();
            for (const auto& [var_, coeff_] : lhs) {
                full_row->affine().linear().set(var_, coeff_);
            }
            // copy the rhs
            full_row->affine().constant() -= rhs_quad.affine().constant();
            for (const auto& [var_, coeff_] : rhs_quad.affine().linear()) {
                full_row->affine().constant() += -coeff_ * var_;
            }
            for (const auto& [pair_, coeff_] : rhs_quad) {
                full_row->affine().constant() += -coeff_ * pair_.first * pair_.second;
            }

        }

        if (first_is_in_sub_problem && second_is_in_sub_problem) {
            full_row->set({pair.first, pair.second}, coeff);
            continue;
        }

        if (first_is_in_sub_problem) {
            auto current_coeff = full_row->affine().linear().get(pair.first);
            full_row->affine().linear().set(pair.first, std::move(current_coeff) + coeff * pair.second);
            continue;
        }

        if (second_is_in_sub_problem) {
            auto current_coeff = full_row->affine().linear().get(pair.second);
            full_row->affine().linear().set(pair.second, std::move(current_coeff) + coeff * pair.first);
            continue;
        }

        full_row->affine().constant() += coeff * pair.first * pair.second;

    }

    auto& sub_problem = m_sub_problems[t_sub_problem_id];
    std::optional<Ctr> new_linear_ctr;

    if (!full_row) {

        // here, we are still on RHS fixation

        if (rhs_quad.empty_all()) {
            new_linear_ctr = sub_problem.model.add_ctr(TempCtr(LinExpr(lhs), type, rhs_quad.affine().constant()), t_ctr.name());
        } else {
            new_linear_ctr = sub_problem.model.add_ctr(TempCtr(std::move(lhs), type, 0), t_ctr.name());
            sub_problem.rhs_fixations.emplace_back(*new_linear_ctr, std::move(rhs_quad));
        }

    } else {

        // here, we are on LHS fixation

        if (full_row->has_quadratic()) {
            // we have to add a quadratic constraint here
            const auto new_quad_ctr = sub_problem.model.add_qctr(QuadExpr(), type, t_ctr.name());
            sub_problem.row_fixations.emplace_back(new_quad_ctr, std::move(*full_row));
            new_quad_ctr.set(*m_initial_penalty_parameters, t_ctr.get(*m_initial_penalty_parameters));
        } else {
            new_linear_ctr = sub_problem.model.add_ctr(TempCtr(LinExpr<Var>(), type, 0.), t_ctr.name());
            sub_problem.row_fixations.emplace_back(*new_linear_ctr, std::move(*full_row));
        }

    }

    if (new_linear_ctr.has_value() && m_initial_penalty_parameters.has_value()) {
        new_linear_ctr->set(*m_initial_penalty_parameters, t_ctr.get(*m_initial_penalty_parameters));
    }

}

double idol::ADM::Formulation::evaluate(const QuadExpr<Var>& t_expr, const std::vector<Point<Var>>& t_primals) {
    double result = t_expr.affine().constant();
    for (const auto& [var, coeff] : t_expr.affine().linear()) {
        result += coeff * t_primals[var.get(m_decomposition)].get(var);
    }
    for (const auto& [pair, coeff] : t_expr) {
        result += coeff * t_primals[pair.first.get(m_decomposition)].get(pair.first) * t_primals[pair.second.get(m_decomposition)].get(pair.second);
    }
    return result;
}

idol::QuadExpr<idol::Var>
idol::ADM::Formulation::evaluate(const QuadExpr<Var, QuadExpr<Var>>& t_expr, const std::vector<Point<Var>>& t_primals) {
    QuadExpr<Var> result;
    result.affine().constant() = evaluate(t_expr.affine().constant(), t_primals);
    for (const auto& [var, coeff] : t_expr.affine().linear()) {
        result.affine().linear().set(var, evaluate(coeff, t_primals));
    }
    for (const auto& [pair, coeff] : t_expr) {
        result.set(pair, evaluate(coeff, t_primals));
    }
    return result;
}

idol::LinExpr<idol::Var> idol::ADM::Formulation::add_l1_vars(unsigned int t_ctr_id, double t_initial_penalty_parameter, idol::CtrType t_type, unsigned int t_sub_problem_id) {

    auto& model = m_sub_problems[t_sub_problem_id].model;

    LinExpr<Var> result;

    const auto get_or_create_l1_var = [&](double t_coeff)-> Var {

        std::optional<Var> var;
        if (const auto it = m_l1_epigraph_vars.find(t_ctr_id) ; it != m_l1_epigraph_vars.end()) {
            var = it->second;
        } else {
            var = Var(model.env(), 0, Inf, Continuous, 0, "l1_epigraph_" + std::to_string(t_ctr_id));
            var->set(*m_initial_penalty_parameters, t_initial_penalty_parameter);
            m_l1_epigraph_vars.emplace_hint(it, t_ctr_id, *var);
            m_sub_problems[t_sub_problem_id].l1_epigraph_vars.emplace_back(*var);
        }

        model.add(*var);
        m_l1_epigraph_vars.emplace(t_ctr_id, *var);
        m_sub_problems[t_sub_problem_id].l1_epigraph_vars.emplace_back(*var);
        result += t_coeff * *var;

        return *var;
    };

    switch (t_type) {
        case Equal: {
            const auto s = get_or_create_l1_var(0);
            const auto s_minus = model.add_var(0, Inf, Continuous, 0);
            const auto s_plus = model.add_var(0, Inf, Continuous, 0);
            model.add_ctr(s == s_minus - s_plus);
            result += s_plus - s_minus;
        } break;
        case LessOrEqual: get_or_create_l1_var(-1); break;
        case GreaterOrEqual: get_or_create_l1_var(1); break;
        default: throw Exception("Unsupported constraint type");
    }

    return result;
}

idol::LinExpr<idol::Var>
idol::ADM::Formulation::add_l1_vars(const idol::Ctr &t_ctr, idol::CtrType t_type, unsigned int t_sub_problem_id) {
    return add_l1_vars(t_ctr.id(), t_ctr.get(*m_initial_penalty_parameters), t_type, t_sub_problem_id);
}

idol::LinExpr<idol::Var>
idol::ADM::Formulation::add_l1_vars(const idol::QCtr &t_ctr, idol::CtrType t_type, unsigned int t_sub_problem_id) {
    return add_l1_vars(t_ctr.id(), t_ctr.get(*m_initial_penalty_parameters), t_type, t_sub_problem_id);
}

idol::ADM::Formulation::SubProblem::SubProblem(idol::Env &t_env) : model(t_env) {

}
