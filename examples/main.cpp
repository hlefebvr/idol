#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"

class MySolver : public Listener {
public:
    explicit MySolver(Model& t_model) {
        t_model.add_listener(*this);
    }
protected:
    void on_start() override {
        std::cout << "start" << std::endl;
    }

    void on_add(const Var &t_var) override {
        std::cout << "adding " << t_var << std::endl;
    }

    void on_add(const Ctr &t_ctr) override {
        std::cout << "adding " << t_ctr << std::endl;
    }
};

std::ostream& operator<<(std::ostream& t_os, const Solution& t_solution) {
    t_os << "SOLUTION STATUS: " << t_solution.status() << '\n';
    if (t_solution.has_value()) {
        t_os << "OBJECTIVE VALUE: " << t_solution.value() << '\n';
    }
    if (t_solution.has_primal_values()) {
        std::cout << "PRIMAL VALUES:" << '\n';
        for (const auto& [var, value] : t_solution.primal_values()) {
            std::cout << '\t' << var << " = " << value << '\n';
        }
    }
    if (t_solution.has_dual_values()) {
        std::cout << "DUAL VALUES:" << '\n';
        for (const auto& [ctr, value] : t_solution.dual_values()) {
            std::cout << '\t' << "dual(" << ctr.name() << ") = " << value << '\n';
        }
    }
    if (t_solution.has_reduced_costs()) {
        std::cout << "REDUCED COSTS:" << '\n';
        for (const auto& [var, value] : t_solution.reduced_costs()) {
            std::cout << '\t' << "rc(" << var << ") = " << value << '\n';
        }
    }
    return t_os;
}

int main() {

    Env env;

    Model model(env);

    auto x = model.add_variable(0., Inf, Continuous, -143, "x");
    auto y = model.add_variable(0., Inf, Continuous, -60, "y");
    model.add_constraint(120 * x + 210 * y <= 15000, "c1");
    model.add_constraint(110 * x + 30 * y <= 4000, "c2");
    model.add_constraint(x + y <= 75, "c3");

    Gurobi gurobi(model);
    gurobi.solve();
    auto solution = gurobi.solution(true, true, true);

    std::cout << solution << std::endl;

    Lpsolve lpsolve(model);
    lpsolve.solve();
    lpsolve.write("model.lp");


    solution = lpsolve.solution(true, true, true);

    std::cout << "\n\n";

    std::cout << solution << std::endl;

    return 0;
}

