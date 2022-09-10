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

int main() {

    Env env;

    Model model(env);

    auto x = model.add_variable(0., 1., Binary, -1, "x");
    auto y = model.add_variable(0., 1., Binary, -2, "y");

    auto ctr = model.add_constraint(x + y <= 1);

    Lpsolve lpsolve(model);
    lpsolve.write("model.lp");
    lpsolve.solve();

    /* Gurobi gurobi(model);
    gurobi.write("model.model");
    gurobi.solve(); */

    return 0;
}

