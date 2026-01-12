//
// Created by henri on 09.01.26.
//
#include <iostream>
#include <idol/modeling.h>

#include "idol/bilevel/modeling/Description.h"
#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation-algorithm/CriticalValueColumnAndConstraintGeneration.h"

using namespace idol;

struct Instance {
    const std::vector<double> weights = {4, 7, 2, 9, 5, 3, 8, 6, 10, 1, 12, 11, 3, 7, 5};
    const std::vector<double> profits = {20, 35, 15, 40, 25, 18, 45, 30, 50, 12, 48, 42, 17, 33, 27};
    const unsigned int n_items = weights.size();
    const double knapsack_capacity = 50;
    const double defender_capacity = 2;
    const double uncertainty_budget = 1; // std::round(std::sqrt(n_items));
};

void solve_with_critical_value_ccg(Env& t_env, const Instance& t_instance);
void solve_with_mibs(Env& env, const Instance& t_instance);

int main(int t_argc, const char** t_argv) {

    Instance instance;
    Env env;
    solve_with_mibs(env, instance);
    solve_with_critical_value_ccg(env, instance);

    return 0;
}

void solve_with_critical_value_ccg(Env& t_env, const Instance& t_instance) {

    std::cout << "Critical Value CCG" << std::endl;
    std::cout << "------------------" << std::endl;

    const unsigned int n_items = t_instance.n_items;

    // Nominal Problem
    Model model(t_env);
    const auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "x");
    const auto y = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "y");
    const auto c = model.add_ctr(idol_Sum(j, Range(n_items), t_instance.weights[j] * x[j]) <= t_instance.knapsack_capacity);
    model.add_ctr(idol_Sum(j, Range(n_items), y[j]) <= t_instance.defender_capacity);
    model.set_obj_expr(idol_Sum(j, Range(n_items), -t_instance.profits[j] * x[j]));

    // Uncertainty set
    Model uncertainty_set(t_env); // Defining U_R = { (u,x) : x in X, u in U(x) }
    uncertainty_set.add_vector<Var, 1>(y);
    const auto u = uncertainty_set.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "u");
    uncertainty_set.add_ctr(idol_Sum(j, Range(n_items), u[j]) <= t_instance.uncertainty_budget);
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        uncertainty_set.add_ctr(u[j] <= 1 - y[j]);
    }

    // Robust description
    Robust::Description description(uncertainty_set);
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        description.set_uncertain_mat_coeff(c, x[j], std::ceil(.5 * t_instance.weights[j]) * u[j]);
    }

    // Print robust model
    // std::cout << Robust::Description::View(model, description) << std::endl;

    // Set optimizer
    auto lozano_borrero = Robust::CriticalValueColumnAndConstraintGeneration(description);
    lozano_borrero.with_master_optimizer(Cplex().with_logs(false));
    lozano_borrero.with_adversarial_optimizer(Cplex().with_logs(false));
    lozano_borrero.with_logs(true);

    model.use(lozano_borrero);

    // Optimize
    model.optimize();

    std::cout << "Status: " << model.get_status() << std::endl;
    std::cout << "Objective: " << model.get_best_obj() << std::endl;

}

void solve_with_mibs(Env& t_env, const Instance& t_instance) {

    std::cout << "Bilevel with MibS" << std::endl;
    std::cout << "-----------------" << std::endl;

    const unsigned int n_items = t_instance.n_items;

    // Bilevel description
    Bilevel::Description description(t_env);

    // Single-level relaxation Problem
    Model model(t_env);

    // Leader's variables
    const auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "x");
    const auto y = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "y");

    // Follower's variables
    const auto u = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "u");
    const auto w = model.add_vars(Dim<1>(n_items), 0, 1, Binary, 0, "w"); // products u * x
    for (const auto& u_j : u) { description.make_lower_level(u_j); }
    for (const auto& w_j : w) { description.make_lower_level(w_j); }

    // Leader's constraints
    model.set_obj_expr(idol_Sum(j, Range(n_items), -t_instance.profits[j] * x[j]));
    model.add_ctr(idol_Sum(j, Range(n_items), t_instance.weights[j] * x[j] + std::ceil(.5 * t_instance.weights[j]) * w[j]) <= t_instance.knapsack_capacity);
    model.add_ctr(idol_Sum(j, Range(n_items), y[j]) <= t_instance.defender_capacity);

    // Follower's constraints
    const auto c = model.add_ctr(idol_Sum(j, Range(n_items), u[j]) <= t_instance.uncertainty_budget);
    description.make_lower_level(c);
    for (unsigned int j = 0 ; j < n_items ; ++j) {

        const auto interdiction = model.add_ctr(u[j] <= 1 - y[j]);
        description.make_lower_level(interdiction);

        const auto mcCormick1 = model.add_ctr(w[j] <= x[j]);
        description.make_lower_level(mcCormick1);

        const auto mcCormick2 = model.add_ctr(w[j] <= u[j]);
        description.make_lower_level(mcCormick2);

        const auto mcCormick3 = model.add_ctr(w[j] >= u[j] + x[j] - 1);
        description.make_lower_level(mcCormick3);

    }
    description.set_lower_level_obj(idol_Sum(j, Range(n_items), -std::ceil(.5 * t_instance.weights[j]) * w[j]));

    // Set optimizer
    auto mibs = Bilevel::MibS(description);
    mibs.with_cplex_for_feasibility(true);
    mibs.with_logs(true);
    model.use(mibs);

    // Optimize
    model.optimize();

    std::cout << "Status: " << model.get_status() << std::endl;
    std::cout << "Objective: " << model.get_best_obj() << std::endl;

}