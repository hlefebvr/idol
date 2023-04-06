//
// Created by henri on 05/04/23.
//
#include "optimizers/solvers/Optimizers_KnapsackSolver.h"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"

Optimizers::KnapsackSolver::KnapsackSolver(const Model &t_model) : Optimizer(t_model) {

}

void Optimizers::KnapsackSolver::hook_optimize() {
    auto instance = create_instance();
    auto result = knapsacksolver::dynamic_programming_primal_dual(instance);
    m_solution = std::make_unique<knapsacksolver::Solution>(std::move(result.solution));
}

std::string Optimizers::KnapsackSolver::name() const {
    return "knapsack-solver";
}

void Optimizers::KnapsackSolver::build() {

}

void Optimizers::KnapsackSolver::add(const Var &t_var) {

}

void Optimizers::KnapsackSolver::add(const Ctr &t_ctr) {

}

void Optimizers::KnapsackSolver::remove(const Var &t_var) {

}

void Optimizers::KnapsackSolver::remove(const Ctr &t_ctr) {

}

void Optimizers::KnapsackSolver::update() {

}

void Optimizers::KnapsackSolver::write(const std::string &t_name) {
    throw Exception("Not implemented.");
}

knapsacksolver::Instance Optimizers::KnapsackSolver::create_instance() {

    const auto& model = parent();

    if (model.ctrs().size() != 1) {
        throw Exception("Given model does not have a knapsack structure.");
    }

    if (!model.get(Attr::Obj::Expr).quadratic().empty()) {
        throw Exception("KnapsackSolver cannot handle quadratic objective functions.");
    }

    m_max_min_factor = model.get(Attr::Obj::Sense) == Minimize ? -1 : 1;

    knapsacksolver::Instance result;

    std::vector<int> fixations;
    fixations.reserve(model.vars().size());

    const auto& knapsack_constraint = *model.ctrs().begin();

    for (const auto& var : model.vars()) {

        const auto& column = model.get(Attr::Var::Column, var);
        const auto& lb = model.get(Attr::Var::Lb, var);
        const auto& ub = model.get(Attr::Var::Ub, var);
        const auto& type = model.get(Attr::Var::Type, var);

        if (type != Binary) {
            throw Exception("Only binary variables are allowed in KnapsackSolver.");
        }

        const auto weight = (long int) (m_int_double_factor * as_numeric(column.linear().get(knapsack_constraint)));
        const auto profit = (long int) (m_int_double_factor * m_max_min_factor * as_numeric(column.obj()));

        result.add_item(weight, profit);

    }

    const auto capacity = (long int) (m_int_double_factor * as_numeric(model.get(Attr::Ctr::Rhs, knapsack_constraint)));
    result.set_capacity(capacity);

    return result;
}

void Optimizers::KnapsackSolver::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {

}

double Optimizers::KnapsackSolver::get(const Req<double, void> &t_attr) const {

    if (t_attr == Attr::Solution::ObjVal) {
        return as_numeric(parent().get(Attr::Obj::Const)) + (m_max_min_factor * m_solution->profit() / m_int_double_factor);
    }

    return Base::get(t_attr);
}

int Optimizers::KnapsackSolver::get(const Req<int, void> &t_attr) const {

    if (t_attr == Attr::Solution::Status) {
        return Optimal;
    }

    if (t_attr == Attr::Solution::Reason) {
        return Proved;
    }

    return Base::get(t_attr);
}

double Optimizers::KnapsackSolver::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Solution::Primal) {
        const auto index = parent().get(Attr::Var::Index, t_var);
        return m_solution->contains_idx(index);
    }

    return Base::get(t_attr, t_var);
}
