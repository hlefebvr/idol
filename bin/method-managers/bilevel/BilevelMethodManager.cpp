//
// Created by Henri on 20/05/2026.
//

#include "BilevelMethodManager.h"

#include "KKT_BigM.h"
#include "KKT_NLP.h"
#include "KKT_SOS1.h"
#include "MibS.h"
#include "PADM.h"
#include "StrongDuality.h"
#include "idol/bilevel/optimizers/wrappers/MibS/Optimizers_MibS.h"
#include "idol/general/utils/SilentMode.h"

BilevelMethodManager::BilevelMethodManager(const Arguments& t_args) : AbstractMethodManager(t_args) {

    add<BilevelMethods::MibS>();
    add<BilevelMethods::KKT_NLP>();
    add<BilevelMethods::KKT_SOS1>();
    add<BilevelMethods::KKT_BigM>();
    add<BilevelMethods::StrongDuality>();
    add<BilevelMethods::PADM>();

}

void BilevelMethodManager::set_problem(const idol::Model& t_model, const idol::Bilevel::Description& t_bilevel_description) {

    m_model = &t_model;
    m_bilevel_description = &t_bilevel_description;
    do_problem_analysis();

    m_problem_has_been_set = true;

    do_method_analysis(other_methods(), false);
}

void BilevelMethodManager::set_problem_spec(const BilevelAnalysisResult& t_result) {

    m_analysis = t_result;

    m_problem_has_been_set = true;

    do_method_analysis(other_methods(), false);

}

std::unique_ptr<idol::OptimizerFactory> BilevelMethodManager::get_optimizer() const {
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

    return method.get_optimizer_factory(*this);
}

void BilevelMethodManager::print_methods(const std::vector<BilevelMethod*>& methods) const {
    return do_method_analysis(methods, true);
}

void BilevelMethodManager::do_problem_analysis() {

    if (!m_model) {
        return;
    }

    // Do leader/follower analysis
    for (const auto& var : m_model->vars()) {

        if (m_bilevel_description->is_upper(var)) {
            do_variable_analysis(m_analysis.leader, *m_model, var);
        } else {
            do_variable_analysis(m_analysis.follower, *m_model, var);
        }

    }

    // Detect coupling and linking constraints
    for (const auto& ctr : m_model->ctrs()) {
        if (m_bilevel_description->is_upper(ctr)) {

            for (const auto& [var, coeff] : m_model->get_ctr_row(ctr)) {
                if (m_bilevel_description->is_lower(var)) {
                    m_analysis.has_coupling_constraints = true;
                    break;
                }
            }

        } else {

            for (const auto& [var, coeff] : m_model->get_ctr_row(ctr)) {
                if (m_bilevel_description->is_lower(var)) {
                    continue;
                }
                const auto type = m_model->get_var_type(var);
                const double lb = m_model->get_var_lb(var);
                const double ub = m_model->get_var_ub(var);
                if (type == idol::Continuous) {
                    m_analysis.has_continuous_linking_variables = true;
                } else if (type == idol::Binary) {
                    m_analysis.has_binary_linking_variables = true;
                } else if (type == idol::Integer) {
                    if (lb >= -.5 && ub <= 1.5) {
                        m_analysis.has_binary_linking_variables = true;
                    } else {
                        m_analysis.has_general_integer_linking_variables = true;
                    }
                }
            }

        }
    }

}

void BilevelMethodManager::do_method_analysis(const std::vector<BilevelMethod*>& methods, bool t_logs) const {

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
            if (condition.requires_continuous_follower) {
                CHECK(
                    !m_analysis.follower.has_binary && !m_analysis.follower.has_general_integer,
                    "Requires a continuous follower problem."
                );
            }
            if (condition.requires_integer_linking_variables) {
                CHECK(
                    m_analysis.has_general_integer_linking_variables || !m_analysis.has_binary_linking_variables,
                    "Requires linking variables to be integer."
                )
            }
            if (condition.requires_big_M_file) {
                CHECK(
                    !m_arguments.bound_provider.empty(),
                    "Requires user-provided bounds (with the --bound-provider option)."
                )
            }
            if (condition.requires_MibS) {
                CHECK(
                    idol::Optimizers::Bilevel::MibS::is_available(),
                    "Requires MibS to be linked."
                )
            }
            if (condition.requires_binary_linking_variables) {
                CHECK(
                    !m_analysis.has_general_integer_linking_variables && !m_analysis.has_continuous_linking_variables,
                    "Requires linking variables to be binary."
                )
            }
            is_applicable |= condition_is_met;
        }
        method->set_applicable(is_applicable);
        std::cout << "\n";
    }

}
