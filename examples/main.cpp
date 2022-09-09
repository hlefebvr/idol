#include <iostream>
#include "modeling.h"

class MyListener : public ModelListener {
public:
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

    MyListener listener;
    model.add_listener(listener);

    auto xi_1 = model.add_parameter(0., 1., Continuous, "xi_1");
    auto xi_2 = model.add_parameter(0., 1., Continuous, "xi_2");
    auto x = model.add_variable(0., 1., Binary, 0, "x");
    auto y = model.add_variable(0., 1., Binary, 0, "y");

    auto expr = x + y;

    std::cout << (x + y) << std::endl;
    std::cout << (2 * x + y) << std::endl;
    std::cout << (xi_1 * x + y) << std::endl;
    std::cout << ((1 + 2 * xi_1) * x + y) << std::endl;
    std::cout << model.add_constraint((1 + 2 * xi_1) * x + y <= 3) << std::endl;

    std::cout << model.objective() << std::endl;

    return 0;
}

