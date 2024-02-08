//
// Created by henri on 08.02.24.
//

#include "idol/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"

idol::Optimizers::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const idol::Model &t_parent,
                                                                               const idol::Model &t_uncertainty_set,
                                                                               const idol::OptimizerFactory &t_master_optimizer,
                                                                               const idol::ColumnAndConstraintGenerationSeparator &t_separator,
                                                                               const idol::Annotation<idol::Var, unsigned int> &t_lower_level_variables,
                                                                               const idol::Annotation<idol::Ctr, unsigned int> &t_lower_level_constraints)
        : Algorithm(t_parent),
          m_uncertainty_set(t_uncertainty_set.copy()),
          m_lower_level_variables(t_lower_level_variables),
          m_lower_level_constraints(t_lower_level_constraints),
          m_master_problem(t_parent.env())
{

    build_master_problem();
    build_coupling_constraints_list();

}

void idol::Optimizers::ColumnAndConstraintGeneration::build_master_problem() {

    build_master_problem_variables();
    build_master_problem_constraints();
    build_master_problem_objective_and_epigraph();

    std::cout << m_master_problem << std::endl;

}

void idol::Optimizers::ColumnAndConstraintGeneration::build_master_problem_variables() {

    const auto& parent = this->parent();

    for (const auto& var : parent.vars()) {

        if (var.get(m_lower_level_variables) != MasterId) {
            continue;
        }

        const double lb = parent.get_var_lb(var);
        const double ub = parent.get_var_ub(var);
        const auto type = parent.get_var_type(var);

        m_master_problem.add(var, TempVar(lb, ub, type, Column()));

    }

}

void idol::Optimizers::ColumnAndConstraintGeneration::build_master_problem_constraints() {

    const auto& parent = this->parent();

    for (const auto& ctr : parent.ctrs()) {

        if (ctr.get(m_lower_level_constraints) != MasterId) {
            continue;
        }

        const auto& row = parent.get_ctr_row(ctr);

        if (contains_lower_level_variable(row.linear()) || contains_lower_level_variable(row.quadratic())) {
            continue;
        }

        const auto type = parent.get_ctr_type(ctr);

        m_master_problem.add(ctr, TempCtr(Row(row), type));

    }

}

void idol::Optimizers::ColumnAndConstraintGeneration::build_master_problem_objective_and_epigraph() {

    const auto& parent = this->parent();
    auto& env = m_master_problem.env();

    const auto& objective = parent.get_obj_expr();

    Expr upper_level_variables_part = objective.constant();
    Expr lower_level_variables_part;
    for (const auto& [var, constant] : objective.linear()) {

        if (var.get(m_lower_level_variables) == MasterId) {
            upper_level_variables_part += constant * var;
        } else {
            lower_level_variables_part += constant * var;
        }

    }

    for (const auto& [var1, var2, constant] : objective.quadratic()) {

        const auto id1 = var1.get(m_lower_level_variables);
        const auto id2 = var2.get(m_lower_level_variables);

        if (id1 != MasterId || id2 != MasterId) {
            lower_level_variables_part += constant * var1 * var2;
        } else {
            upper_level_variables_part += constant * var1 * var2;
        }

    }

    m_master_problem.set_obj_expr(upper_level_variables_part);

    if (lower_level_variables_part.is_zero()) {
        return;
    }

    auto epigraph_variable = Var(env, -Inf, Inf, Continuous, "epigraph");
    m_epigraph_constraint = std::make_optional<Ctr>(env, epigraph_variable >= lower_level_variables_part);

}

void idol::Optimizers::ColumnAndConstraintGeneration::build_coupling_constraints_list() {

    const auto& parent = this->parent();

    m_coupling_constraints.clear();

    if (m_epigraph_constraint.has_value()) {
        m_coupling_constraints.emplace_back(*m_epigraph_constraint);
    }

    for (const auto& ctr : parent.ctrs()) {

        if (ctr.get(m_lower_level_constraints) != MasterId) {
            continue;
        }

        const auto& row = parent.get_ctr_row(ctr);

        if (!contains_lower_level_variable(row.linear()) && !contains_lower_level_variable(row.quadratic())) {
            continue;
        }

        m_coupling_constraints.emplace_back(ctr);

    }

}


std::string idol::Optimizers::ColumnAndConstraintGeneration::name() const {
    return "column-and-constraint generation";
}

double idol::Optimizers::ColumnAndConstraintGeneration::get_var_primal(const Var &t_var) const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

double idol::Optimizers::ColumnAndConstraintGeneration::get_var_ray(const Var &t_var) const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

double idol::Optimizers::ColumnAndConstraintGeneration::get_ctr_dual(const Ctr &t_ctr) const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

double idol::Optimizers::ColumnAndConstraintGeneration::get_ctr_farkas(const Ctr &t_ctr) const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

unsigned int idol::Optimizers::ColumnAndConstraintGeneration::get_n_solutions() const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

unsigned int idol::Optimizers::ColumnAndConstraintGeneration::get_solution_index() const {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::add(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::add(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::remove(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::remove(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update() {

}

void idol::Optimizers::ColumnAndConstraintGeneration::write(const std::string &t_name) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::hook_before_optimize() {
    Optimizer::hook_before_optimize();

    if (get_param_logs()) {

        const auto& parent = this->parent();

        const unsigned int n_master_constraints = m_master_problem.ctrs().size();
        const unsigned int n_coupling_constraints = m_coupling_constraints.size();
        const unsigned int n_lower_level_constraints = parent.ctrs().size() - n_master_constraints - n_coupling_constraints;
        const unsigned int n_upper_level_variables = m_master_problem.vars().size();
        const unsigned int n_lower_level_variables = parent.vars().size() - n_upper_level_variables;

        std::cout << "------------------------------------\n";
        std::cout << "* Solving Two-Stage Robust Problem *\n";
        std::cout << "------------------------------------\n\n";

        std::cout << "Variables: \n";
        std::cout << "\tN. upper level: " << n_upper_level_variables << '\n';
        std::cout << "\tN. lower level: " << n_lower_level_variables << '\n';

        std::cout << "Constraints: \n";
        std::cout << "\tN. pure upper level: " << n_master_constraints << '\n';
        std::cout << "\tN. coupling: " << n_coupling_constraints << '\n';
        std::cout << "\tN. lower level: " << n_lower_level_constraints << '\n';
    }

}

void idol::Optimizers::ColumnAndConstraintGeneration::hook_optimize() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::set_solution_index(unsigned int t_index) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_obj_sense() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_obj() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_rhs() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_obj_constant() {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_mat_coeff(const Ctr &t_ctr, const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_ctr_type(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_ctr_rhs(const Ctr &t_ctr) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_var_type(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_var_lb(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_var_ub(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

void idol::Optimizers::ColumnAndConstraintGeneration::update_var_obj(const Var &t_var) {
    throw Exception("Not implemented: Optimizers::ColumnAndConstraintGeneration::" + std::string(__FUNCTION__) );
}

bool
idol::Optimizers::ColumnAndConstraintGeneration::contains_lower_level_variable(const idol::LinExpr<idol::Var> &t_expr) {

    for (const auto& [var, constant] : t_expr) {

        if (var.get(m_lower_level_variables) != MasterId) {
            return true;
        }

    }

    return false;
}

bool idol::Optimizers::ColumnAndConstraintGeneration::contains_lower_level_variable(
        const idol::QuadExpr<idol::Var, idol::Var> &t_expr) {


    for (const auto& [var1, var2, constant] : t_expr) {

        if (var1.get(m_lower_level_variables) != MasterId || var2.get(m_lower_level_variables) != MasterId) {
            return true;
        }

    }

    return false;

}
