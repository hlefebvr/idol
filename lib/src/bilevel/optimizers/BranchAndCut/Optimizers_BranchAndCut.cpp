#include "idol/bilevel/optimizers/BranchAndCut/Optimizers_BranchAndCut.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include <cassert>

#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/bilevel/optimizers/wrappers/MibS/Optimizers_MibS.h"

idol::Optimizers::Bilevel::BranchAndCut::BranchAndCut(const idol::Model &t_parent,
                                                      const idol::Bilevel::Description &m_description,
                                                      const idol::OptimizerFactory &t_optimizer_for_sub_problems)
                                                      : Algorithm(t_parent),
                                                        m_description(m_description),
                                                        m_optimizer_for_sub_problems(t_optimizer_for_sub_problems.clone())
                                                      {

}

std::string idol::Optimizers::Bilevel::BranchAndCut::name() const {
    return "bilevel::branch-and-cut";
}

double idol::Optimizers::Bilevel::BranchAndCut::get_var_primal(const Var &t_var) const {
    return m_hpr->get_var_primal(t_var);
}

double idol::Optimizers::Bilevel::BranchAndCut::get_var_reduced_cost(const Var &t_var) const {
    return m_hpr->get_var_reduced_cost(t_var);
}

double idol::Optimizers::Bilevel::BranchAndCut::get_var_ray(const Var &t_var) const {
    return m_hpr->get_var_ray(t_var);
}

double idol::Optimizers::Bilevel::BranchAndCut::get_ctr_dual(const Ctr &t_ctr) const {
    return m_hpr->get_ctr_dual(t_ctr);
}

double idol::Optimizers::Bilevel::BranchAndCut::get_ctr_farkas(const Ctr &t_ctr) const {
    return m_hpr->get_ctr_farkas(t_ctr);
}

unsigned int idol::Optimizers::Bilevel::BranchAndCut::get_n_solutions() const {
    return m_hpr->get_n_solutions();
}

unsigned int idol::Optimizers::Bilevel::BranchAndCut::get_solution_index() const {
    return m_hpr->get_solution_index();
}

void idol::Optimizers::Bilevel::BranchAndCut::add(const Var &t_var) {

}

void idol::Optimizers::Bilevel::BranchAndCut::add(const Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::BranchAndCut::add(const QCtr &t_ctr) {

}

void idol::Optimizers::Bilevel::BranchAndCut::remove(const Var &t_var) {

}

void idol::Optimizers::Bilevel::BranchAndCut::remove(const Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::BranchAndCut::remove(const QCtr &t_ctr) {

}

void idol::Optimizers::Bilevel::BranchAndCut::update() {
    m_hpr.reset();
}

void idol::Optimizers::Bilevel::BranchAndCut::write(const std::string &t_name) {
    m_hpr->write(t_name);
}

void idol::Optimizers::Bilevel::BranchAndCut::set_solution_index(unsigned int t_index) {
    m_hpr->set_solution_index(t_index);
}

void idol::Optimizers::Bilevel::BranchAndCut::update_obj_sense() {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_obj() {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_rhs() {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_obj_constant() {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_mat_coeff(const Ctr &t_ctr, const Var &t_var) {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_ctr_type(const Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_ctr_rhs(const Ctr &t_ctr) {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_var_type(const Var &t_var) {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_var_lb(const Var &t_var) {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_var_ub(const Var &t_var) {

}

void idol::Optimizers::Bilevel::BranchAndCut::update_var_obj(const Var &t_var) {

}

void idol::Optimizers::Bilevel::BranchAndCut::check_value_function_is_well_posed() {

    const auto& hpr = parent();
    auto& env = hpr.env();
    Model model(env);

    for (const auto& var : hpr.vars()) {
        if (m_description.is_upper(var)) {
            continue;
        }
        const double lb = hpr.get_var_lb(var);
        const double ub = hpr.get_var_ub(var);
        const auto type = hpr.get_var_type(var);

        double delta_lb = -1, delta_ub = 1;
        if (!is_pos_inf(ub)) {
            delta_ub = 0;
        }
        if (!is_neg_inf(lb)) {
            delta_lb = 0;
        }
        model.add(var, TempVar(delta_lb, delta_ub, type, 0., LinExpr<Ctr>()));
    }

    for (const auto& ctr : hpr.ctrs()) {
        if (m_description.is_upper(ctr)) {
            continue;
        }
        LinExpr<Var> lhs;
        for (const auto& [var, coeff] : hpr.get_ctr_row(ctr)) {
            if (m_description.is_upper(var)) {
                continue;
            }
            lhs += coeff * var;
        }
        const auto type = hpr.get_ctr_type(ctr);
        model.add_ctr(TempCtr(std::move(lhs), type, 0));
    }

    model.set_obj_expr(m_description.lower_level_obj());
    model.use(*m_optimizer_for_sub_problems);
    model.optimize();

    if (model.get_best_obj() < 0) {
        set_status(Infeasible);
        set_reason(Proved);
        set_best_obj(Inf);
        set_best_bound(Inf);
        terminate();
    }

}

void idol::Optimizers::Bilevel::BranchAndCut::check_assumptions() {

    const auto& hpr = parent();

    std::vector<Var> upper_variables;
    upper_variables.reserve(hpr.vars().size());
    for (const auto& var : hpr.vars()) {
        if (m_description.is_upper(var)) {
            upper_variables.emplace_back(var);
        }
    }

    // Build list of linking variables
    m_linking_upper_variables.clear();
    m_linking_upper_variables.reserve(upper_variables.size());
    for (const auto& var : upper_variables) {
        bool is_linking = false;
        for (const auto &[ctr, coeff]: hpr.get_var_column(var)) {
            if (m_description.is_lower(ctr)) {
                is_linking = true;
                break;
            }
        }
        if (is_linking) {
            m_linking_upper_variables.emplace_back(var);
        }
    }

    // Check all linking variables are binary
    for (const auto& var : m_linking_upper_variables) {

        const double lb = hpr.get_var_lb(var);
        const double ub = hpr.get_var_ub(var);
        const auto type = hpr.get_var_type(var);

        if (!(type == Binary || (type == Integer && lb > -0.5 && ub < 1.5))) {
            throw Exception("Linking variable must be binary.");
        }

    }

}

void idol::Optimizers::Bilevel::BranchAndCut::build_separation_problem() {

    const auto& hpr = parent();
    m_separation_problem = std::make_unique<Model>(hpr.copy());

    for (const auto& c : hpr.ctrs()) {

        if (m_description.is_lower(c)) {
            continue;
        }

        for (const auto& [var, coeff] : hpr.get_ctr_row(c)) {
            if (m_description.is_lower(var)) { // it is an upper-level constraint with a lower-level variable, i.e., it's  a coupling constraint
                m_separation_problem->remove(c);
                break;
            }
        }

    }

}

std::tuple<double, double, double, double>
idol::Optimizers::Bilevel::BranchAndCut::compute_big_M(const idol::Model &t_model) {

    double trivial_LB = -Inf, trivial_UB = Inf;
    double elaborated_LB = -Inf, elaborated_UB = Inf;

    // Compute trivial bounds
    for (const auto& [var, constant] : m_description.lower_level_obj().affine().linear()) {

        const double lb = t_model.get_var_lb(var);
        const double ub = t_model.get_var_ub(var);

        if (is_pos_inf(ub) || is_neg_inf(lb)) {
            trivial_LB = -Inf;
            trivial_UB = Inf;
            break;
        }

        if (constant > 0) {
            trivial_UB += constant * ub;
            trivial_LB += constant * lb;
        } else {
            trivial_UB += constant * lb;
            trivial_LB += constant * ub;
        }

    }

    if (true) { // TODO Only if an optimizer for this was given

        auto copy = t_model.copy();
        copy.use(*m_optimizer_for_sub_problems);
        copy.optimizer().set_param_time_limit(5 * 60);

        copy.set_obj_expr(m_description.lower_level_obj());
        copy.optimize();

        elaborated_LB = is_inf(copy.get_best_bound()) ? -Inf : copy.get_best_bound();

        copy.set_obj_expr(-1. * m_description.lower_level_obj());
        copy.optimize();

        elaborated_UB = is_inf(copy.get_best_bound()) ? Inf : -copy.get_best_bound();

    }

    const double LB = std::max(trivial_LB, elaborated_LB);
    const double UB = std::min(trivial_UB, elaborated_UB);

    return {
        LB - UB,
        UB - LB,
        2 * LB - UB,
        2 * UB - LB
    };
}

void idol::Optimizers::Bilevel::BranchAndCut::build_hpr(double t_lambda_lb, double t_lambda_ub, double t_pi_lb, double t_pi_ub) {

    m_hpr = std::make_unique<Model>(parent().copy());
    const unsigned int n = m_linking_upper_variables.size();
    m_lambda = m_hpr->add_vars(Dim<1>(n), t_lambda_lb, t_lambda_ub, Continuous, 0, "lambda");
    m_pi = m_hpr->add_var(t_pi_lb, t_pi_ub, Continuous, 0, "pi");
    const auto w = m_hpr->add_vars(Dim<1>(n), t_lambda_lb, t_lambda_ub, Continuous, 0, "w");

    const auto strong_duality = m_hpr->add_ctr(
            m_description.lower_level_obj().affine().linear() <= *m_pi + idol_Sum(i, Range(n), w[i])
    );

    for (unsigned int i : Range(n)) {
        m_hpr->add_ctr(w[i] <= t_lambda_ub * m_linking_upper_variables[i]);
        m_hpr->add_ctr(w[i] >= t_lambda_lb * m_linking_upper_variables[i]);
        m_hpr->add_ctr(w[i] <= m_lambda[i] + t_lambda_ub * (1 - m_linking_upper_variables[i]));
        m_hpr->add_ctr(w[i] >= m_lambda[i] + t_lambda_lb * (1 - m_linking_upper_variables[i]));
    }

}


void idol::Optimizers::Bilevel::BranchAndCut::hook_before_optimize() {
    Algorithm::hook_before_optimize();

    m_hpr.reset();
    m_separation_problem.reset();
    m_linking_upper_variables.clear();
    m_pi.reset();
    m_lambda.clear();

    set_status(Loaded);
    set_reason(NotSpecified);
    set_best_bound(-Inf);
    set_best_obj(Inf);

}

void idol::Optimizers::Bilevel::BranchAndCut::hook_optimize() {

    check_assumptions();

    check_value_function_is_well_posed();
    if (is_terminated()) { return; }

    build_separation_problem();
    const auto[lambda_lb, lambda_ub, pi_lb, pi_ub] = compute_big_M(*m_separation_problem);
    //std::cout << "Bounds: " << lambda_lb << ", " << lambda_ub << ", " << pi_lb << ", " << pi_ub << std::endl;

    // Check bounds numerical stability
    for (auto bound : { lambda_lb, lambda_ub, pi_lb, pi_ub }) {
        if (bound > 1e10 || bound < -1e10) {
            std::cout << "Bounds on the dual variables were too large." << std::endl;
            set_status(Fail);
            set_reason(Numerical);
            terminate();
            return;
        }
    }

    build_hpr(lambda_lb, lambda_ub, pi_lb, pi_ub);

    auto cut = build_dantzig_wolfe_cut();
    LazyCutCallback cut_cb(*m_separation_problem, std::move(cut), LessOrEqual);
    cut_cb.with_separation_optimizer(*m_optimizer_for_sub_problems);

    ::idol::Gurobi main_optimizer;
    main_optimizer.with_lazy_cut(true);
    main_optimizer.add_callback(cut_cb);
    //main_optimizer.add_callback(bound_cb);
    main_optimizer.with_logs(get_param_logs());

    auto mibs_model = parent().copy();
    mibs_model.use(::idol::Bilevel::MibS(m_description).with_cplex_for_feasibility(true));
    mibs_model.optimize();

    m_hpr->use(main_optimizer);
    m_hpr->optimize();

    std::cout << "MibS: " << mibs_model.get_best_obj() << std::endl;
    std::cout << "HPR: " << m_hpr->get_best_obj() << std::endl;
    //assert(is_zero(m_hpr->get_best_obj() - mibs_model.get_best_obj(), 1e-3));

    set_status(m_hpr->get_status());
    set_reason(m_hpr->get_reason());
    set_best_obj(m_hpr->get_best_obj());
    set_best_bound(m_hpr->get_best_bound());

}

idol::GenerationPattern<idol::Ctr> idol::Optimizers::Bilevel::BranchAndCut::build_dantzig_wolfe_cut() {

    const auto n = m_linking_upper_variables.size();

    return *m_pi + idol_Sum(i, Range(n), !m_linking_upper_variables[i] * m_lambda[i]) - m_description.lower_level_obj().affine().linear();
}
