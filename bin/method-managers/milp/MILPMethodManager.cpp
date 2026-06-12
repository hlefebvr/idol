//
// Created by Henri on 19/05/2026.
//
#include "MILPMethodManager.h"

#include "Cplex.h"
#include "GLPK.h"
#include "Gurobi.h"
#include "HiGHS.h"
#include "JuMP.h"
#include "idol/general/utils/SilentMode.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/HiGHS/Optimizers_HiGHS.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/JuMP.h"
#include "idol/mixed-integer/optimizers/wrappers/JuMP/Optimizers_JuMP.h"

MILPMethodManager::MILPMethodManager(const Arguments& t_args)
    : AbstractMethodManager<MILPMethod>(t_args) {

    add<MILPMethods::Gurobi>();
    add<MILPMethods::Cplex>();
    add<MILPMethods::GLPK>();
    add<MILPMethods::HiGHS>();
    add<MILPMethods::Cplex>();
    add<MILPMethods::JuMP>();

}

void MILPMethodManager::set_problem(const idol::Model& t_model) {

    m_model = &t_model;
    do_problem_analysis();

    m_problem_has_been_set = true;

    do_method_analysis(other_methods(), false);
}

void MILPMethodManager::set_problem_spec(const MILPAnalysisResult& t_result) {

    m_analysis = t_result;

    m_problem_has_been_set = true;

    do_method_analysis(other_methods(), false);

}

void MILPMethodManager::set_optimizer(idol::Model& t_model) const {
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
    t_model.use(*method.get_optimizer_factory(*this, false));
}

std::unique_ptr<idol::OptimizerFactory>
MILPMethodManager::get_sub_milp_optimizer(const Arguments& t_args, bool t_relax_integrality, const MILPAnalysisResult& t_analysis) {

    MILPMethodManager manager(t_args);
    manager.set_problem_spec(t_analysis);
    const auto& method = manager.get_method(t_args.default_milp_method);

    return method.get_optimizer_factory(manager, t_relax_integrality);
}

void MILPMethodManager::print_methods(const std::vector<MILPMethod*>& methods) const {
    return do_method_analysis(methods, true);
}

void MILPMethodManager::do_problem_analysis() {

    if (!m_model) {
        return;
    }

    for (const auto& var : m_model->vars()) {
        do_variable_analysis(m_analysis, *m_model, var);
    }

    m_analysis.has_quadratic_constraints = m_model->qctrs().size() > 0;
    m_analysis.has_sos_constraints = m_model->sosctrs().size() > 0;
    m_analysis.has_quadratic_objective_function = m_model->get_obj_expr().has_quadratic();

}

void MILPMethodManager::do_method_analysis(const std::vector<MILPMethod*>& methods, bool t_logs) const {

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
            if (condition.requires_gurobi) {
                CHECK(
                    idol::Optimizers::Gurobi::is_available(),
                    "Requires Gurobi to be installed."
                );
            }
            if (condition.requires_glpk) {
                CHECK(
                    idol::Optimizers::GLPK::is_available(),
                    "Requires GLPK to be installed."
                );
            }
            if (condition.requires_highs) {
                CHECK(
                    idol::Optimizers::HiGHS::is_available(),
                    "Requires HiGHS to be installed."
                );
            }
            if (condition.requires_cplex) {
                CHECK(
                    idol::Optimizers::Cplex::is_available(),
                    "Requires Cplex to be installed."
                );
            }
            if (condition.requires_jump) {
                CHECK(
                    idol::Optimizers::JuMP::is_available(),
                    "Requires JuMP to be installed."
                )
            }
            if (condition.requires_no_sos_constraint) {
                CHECK(
                    !m_analysis.has_sos_constraints,
                    "Requires no SOS-type constraints."
                );
            }
            if (condition.requires_no_quadratic_constraint) {
                CHECK(
                    !m_analysis.has_quadratic_constraints,
                    "Requires no quadratic constraints."
                )
            }
            if (condition.requires_jump_optimizer) {
                CHECK(
                    !m_arguments.jump_optimizer.empty(),
                    "Requires a JuMP Optimizer to be specified with --jump-optimizer."
                )
            }
            if (condition.requires_no_quadratic_objective_function) {
                CHECK(
                    !m_analysis.has_quadratic_objective_function,
                    "Requires a linear objective function."
                )
            }
            is_applicable |= condition_is_met;
        }
        method->set_applicable(is_applicable);
        std::cout << "\n";
    }

}

