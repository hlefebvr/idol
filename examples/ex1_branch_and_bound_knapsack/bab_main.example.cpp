//
// Created by henri on 16/09/22.
//
#include <iostream>
#include "modeling.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"

class MyListener : public Listener {
public:
    unsigned int n_lb_update = 0;
    unsigned int n_ub_update = 0;
protected:
    void on_update_lb(const Var &t_var, double t_lb) override {
        ++n_lb_update;
    }

    void on_update_ub(const Var &t_var, double t_ub) override {
        ++n_ub_update;
    }
};

int main() {

    Log::set_level(Trace);
    Log::set_color("ex1_branch_and_bound_knapsack", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    Env env;

    Model model(env);

    const std::vector<std::pair<double, double>> items = { {2, 40}, {3.14, 50}, {1.98, 100}, {5, 95}, {3, 30} }; // (weight, value)
    const double capacity = 10.;

    std::vector<Var> x;
    x.reserve(items.size());

    Expr sum_weight;

    for (const auto& [weight, profit] : items) {
        auto var = model.add_variable(0., 1., Continuous, -profit);
        sum_weight += weight * var;
        x.emplace_back(var);
    }

    model.add_constraint(sum_weight <= capacity);

    MyListener listener;
    model.add_listener(listener);

    BranchAndBound solver(model, x);
    solver.solve();

    std::cout << listener.n_lb_update << std::endl;
    std::cout << listener.n_ub_update << std::endl;

    return 0;
}

