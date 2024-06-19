//
// Created by henri on 06/04/23.
//
#include <iostream>
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/modeling.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/Mosek/Mosek.h"
#include "idol/optimizers/mixed-integer-programming/callbacks/cutting-planes/LazyCutCallback.h"

using namespace idol;

class EarlyStopCallback : public CallbackFactory {
public:
    EarlyStopCallback() = default;

    EarlyStopCallback(const EarlyStopCallback&) = default;
    EarlyStopCallback(EarlyStopCallback&&) = default;

    EarlyStopCallback& operator=(const EarlyStopCallback&) = default;
    EarlyStopCallback& operator=(EarlyStopCallback&&) = default;

    class Strategy : public Callback {
    protected:
        void operator()(CallbackEvent t_event) override {
            std::cout << "Time: " << time().count() << std::endl;
            std::cout << primal_solution() << std::endl;
        }
    };

    Callback *operator()() override {
        return new Strategy();
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new EarlyStopCallback(*this);
    }
};

int main(int t_argc, const char** t_argv) {

    const auto instance = Problems::KP::read_instance("knapsack.data.txt");

    const auto n_items = instance.n_items();

    Env env;

    // Create model
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, "x");

    model.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    // Set optimizer
    model.use(
            Mosek()
                .add_callback(
                        EarlyStopCallback()
                )
            );

    // Solve
    model.optimize();

    std::cout << "Objective value = " << model.get_best_obj() << std::endl;

    std::cout << save_primal(model) << std::endl;

    return 0;
}
