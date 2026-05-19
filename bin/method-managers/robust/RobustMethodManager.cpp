//
// Created by Henri on 19/05/2026.
//

#include "RobustMethodManager.h"

#include <fstream>

#include "CCG_CV.h"
#include "CCG_Farkas.h"
#include "CCG_KKT_SOS1.h"
#include "CCG_MibS.h"
#include "CG_Indicator.h"
#include "idol/general/utils/SilentMode.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/bilevel/optimizers/wrappers/MibS/Optimizers_MibS.h"

RobustMethodManager::RobustMethodManager(const Arguments& t_args) : AbstractMethodManager(t_args) {

    add<RobustMethods::CCG_Farkas>();
    add<RobustMethods::CCG_KKT_SOS1>();
    add<RobustMethods::CCG_MibS>();
    add<RobustMethods::CCG_CV>();
    add<RobustMethods::CG_Indicator>();

}

void RobustMethodManager::set_problem(const idol::Model& t_model, const idol::Robust::Description& t_robust_description, const std::optional<idol::Bilevel::Description>& t_bilevel_description) {

    m_model = &t_model;
    m_robust_description = &t_robust_description;
    if (t_bilevel_description) {
        m_bilevel_description = &t_bilevel_description.value();
    }

    m_problem_has_been_set = true;

    do_stage_analysis();
    do_uncertainty_set_analysis();

    do_method_analysis(other_methods(), false);

}

void RobustMethodManager::set_optimizer(idol::Model& t_model) const {
    std::cout << "-- Applicable methods are [";
    bool is_first = true;
    for (const auto& method : applicable_methods()) {
        if (is_first) {
            is_first = false;
        } else {
            std::cout << ", ";
        }
        std::cout << method->name();
    }
    std::cout << "]\n";
    const auto& method = get_method(m_arguments.method);
    std::cout << "-- Using method " << method.name() << '.' << std::endl;
    method.set_optimizer(t_model, *this);
}

void RobustMethodManager::do_stage_analysis() {

    if (!m_model) {
        return;
    }

    for (const auto& var : m_model->vars()) {

        if (!m_bilevel_description || m_bilevel_description->is_upper(var)) {
            do_variable_analysis(m_stage_analysis.first_stage, *m_model, var);
        } else {
            do_variable_analysis(m_stage_analysis.second_stage, *m_model, var);
            m_stage_analysis.has_second_stage = true;
        }

    }

}

void RobustMethodManager::do_uncertainty_set_analysis() {

    if (!m_robust_description) {
        return;
    }

    const auto& uncertainty_set = m_robust_description->uncertainty_set();

    for (const auto& var : uncertainty_set.vars()) {
        do_variable_analysis(m_uncertainty_set_analysis, uncertainty_set, var);
    }

    for (const auto& var : uncertainty_set.vars()) {

        if (!m_model->has(var)) {
            continue;
        }

        m_uncertainty_set_analysis.has_decision_dependence = true;
        const auto type = uncertainty_set.get_var_type(var);

        if (type != idol::Binary) {
            m_uncertainty_set_analysis.has_binary_linking_variables = false;
        }

        if (type == idol::Continuous) {
            m_uncertainty_set_analysis.has_integer_linking_variables = false;
        }
    }
}

void RobustMethodManager::print_methods(const std::vector<RobustMethod*>& methods) const {
    return do_method_analysis(methods, true);
}

void RobustMethodManager::do_method_analysis(const std::vector<RobustMethod*>& methods, bool t_logs) const {

    idol::SilentMode silent_mode(!t_logs);

    for (const auto& method : methods) {
        std::cout << "\033[1m" << method->name() << "\033[0m\n";
        std::cout << "Description: " << method->description() << "\n";
        std::cout << "Required Assumptions:\n";
        bool is_applicable = false;
        bool is_first = true;
        for (const auto& condition : method->conditions()) {
            if (!is_first) { std::cout << " or," << std::endl; }
            is_first = false;
            bool condition_is_met = true;
            if (condition.requires_single_stage_problem) {
                CHECK(
                    !m_stage_analysis.has_second_stage,
                    "Requires a static robust problem."
                );
            }
            if (condition.requires_two_stage_problem) {
                CHECK(
                    m_stage_analysis.has_second_stage,
                    "Requires a two-stage robust problem."
                );
            }
            if (condition.requires_binary_uncertainty) {
                CHECK(
                    m_uncertainty_set_analysis.has_binary && !m_uncertainty_set_analysis.has_continuous && !m_uncertainty_set_analysis.has_general_integer,
                    "Requires a binary uncertainty set."
                );
            }
            if (condition.requires_continuous_second_stage) {
                CHECK(
                    !m_stage_analysis.second_stage.has_binary && !m_stage_analysis.second_stage.has_general_integer,
                    "Requires only continuous second-stage decisions."
                );
            }
            if (condition.requires_a_0_1_uncertainty_set) {
                CHECK(
                    m_uncertainty_set_analysis.is_zero_one_polytope,
                    "Requires a 0-1 polytope as uncertainty set."
                );
            }
            if (condition.requires_a_solver_with_sos1) {
                CHECK(
                    idol::Optimizers::Gurobi::is_available(),
                    "Requires a solver that handles SOS1 constraints."
                );
            }
            if (condition.requires_mibs) {
                CHECK(
                    idol::Optimizers::Bilevel::MibS::is_available(),
                    "Requires the mixed-integer bilevel solver MibS"
                )
            }
            if (condition.requires_general_integer_uncertainty_set) {
                CHECK(
                    m_uncertainty_set_analysis.has_general_integer || m_uncertainty_set_analysis.has_binary,
                    "Requires integer uncertain parameters."
                )
            }
            if (condition.requires_decision_independent_uncertainty_set) {
                CHECK(
                    !m_uncertainty_set_analysis.has_decision_dependence,
                    "Requires a decision-independent uncertainty set."
                )
            }
            if (condition.requires_decision_dependent_uncertainty_set) {
                CHECK(
                    m_uncertainty_set_analysis.has_decision_dependence,
                    "Requires a decision-dependent uncertainty set."
                )
            }
            if (condition.requires_uncertainty_set_with_integer_coefficients) {
                CHECK(
                    m_uncertainty_set_analysis.has_integer_coefficients,
                    "Requires integer coefficients in the uncertainty set."
                )
            }
            if (condition.requires_integer_linking_variables_in_uncertainty_set) {
                CHECK(
                    m_uncertainty_set_analysis.has_integer_linking_variables || m_uncertainty_set_analysis.has_binary_linking_variables,
                    "Requires integer linking variables in the uncertainty set."
                )
            }
            if (condition.requires_binary_linking_variables_in_uncertainty_set) {
                CHECK(
                    m_uncertainty_set_analysis.has_binary_linking_variables,
                    "Requires binary linking variables in the uncertainty set."
                )
            }
            if (condition.requires_complete_recourse) {
                CHECK(
                    m_arguments.complete_recourse,
                    "Requires complete recourse"
                )
            }
            if (condition.requires_bounded_second_stage) {
                CHECK(
                    m_stage_analysis.second_stage.all_bounded,
                    "Requires bounded second stage."
                )
            }
            is_applicable |= condition_is_met;
        }
        method->set_applicable(is_applicable);
        std::cout << "\n";
    }

}
