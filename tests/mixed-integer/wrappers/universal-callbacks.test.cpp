#include <catch2/catch_all.hpp>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include "idol/mixed-integer/optimizers/callbacks/CallbackFactory.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h"
#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/UserCutCallback.h"

using namespace Catch::literals;
using namespace idol;

namespace {
enum class Action { Observe, UserCut, Terminate };
struct State { unsigned incumbent = 0, fractional = 0; bool read = false, acted = false; };

class TestCallback : public Callback {
    std::shared_ptr<State> state; Var x, y; Action action;
public:
    TestCallback(std::shared_ptr<State> s, Var x_, Var y_, Action a) : state(std::move(s)), x(x_), y(y_), action(a) {}
    void operator()(CallbackEvent event) override {
        if (event == IncumbentSolution) ++state->incumbent;
        if (event == InvalidSolution) ++state->fractional;
        if (event == IncumbentSolution || event == InvalidSolution) {
            (void) primal_solution().get(x); state->read = true;
        }
        if (state->acted) return;
        if (action == Action::UserCut && event == InvalidSolution) {
            add_user_cut(x <= 1); state->acted = true; // Valid because x is integer and 2x <= 3.
        } else if (action == Action::Terminate) {
            terminate(); state->acted = true;
        }
    }
};

class Factory : public CallbackFactory {
    std::shared_ptr<State> state; Var x, y; Action action;
public:
    Factory(std::shared_ptr<State> s, Var x_, Var y_, Action a) : state(std::move(s)), x(x_), y(y_), action(a) {}
    Callback* operator()() override { return new TestCallback(state, x, y, action); }
    CallbackFactory* clone() const override { return new Factory(*this); }
};

auto make_optimizer() { return OPTIMIZER().with_lazy_cut(true).with_presolve(false); }
std::tuple<Model, Var, Var> make_model(Env& env) {
    Model model(env); auto y = model.add_var(0, Inf, Integer, 2, "y"); auto z = model.add_var(0, Inf, Continuous, 1, "z");
    return {std::move(model), y, z};
}

template<class CutCallback, class OptimizerT>
void add_benders_callback(OptimizerT& optimizer, Env& env, const Var& y, const Var& z) {
    Model separation(env);
    const auto lambda = separation.add_vars(Dim<1>(2), 0, Inf, Continuous, 0, "lambda");
    separation.add_ctr(lambda[0] + 2 * lambda[1] <= 2);
    separation.add_ctr(2 * lambda[0] - lambda[1] <= 3);
    const auto cut = z - (!lambda[0] * 3 - !lambda[0] * y + !lambda[1] * 4 - !lambda[1] * 3 * y);
    auto callback = CutCallback(separation, cut, GreaterOrEqual);
    callback.with_separation_optimizer(Gurobi());
    optimizer.add_callback(callback);
}
}

TEST_CASE("Universal callback observes incumbent and fractional solutions", "[Callbacks]") {
    Env env; auto [model, x, y] = make_model(env); auto state = std::make_shared<State>();
    auto optimizer = make_optimizer(); optimizer.add_callback(Factory(state, x, y, Action::Observe));
    add_benders_callback<LazyCutCallback>(optimizer, env, x, y);
    model.use(optimizer); model.optimize();
    CHECK(model.get_status() == Optimal); CHECK(model.get_best_obj() == 5.4_a);
    CHECK(state->incumbent > 0); CHECK(state->fractional > 0); CHECK(state->read);
}

TEST_CASE("Universal callback adds a globally valid user cut", "[Callbacks]") {
    Env env; auto [model, x, y] = make_model(env); auto state = std::make_shared<State>();
    auto optimizer = make_optimizer(); optimizer.add_callback(Factory(state, x, y, Action::Observe));
    add_benders_callback<LazyCutCallback>(optimizer, env, x, y);
    add_benders_callback<UserCutCallback>(optimizer, env, x, y);
    model.use(optimizer); model.optimize();
    CHECK(model.get_status() == Optimal); CHECK(model.get_best_obj() == 5.4_a); CHECK(state->fractional > 0);
}

TEST_CASE("Universal callback adds a lazy constraint", "[Callbacks]") {
    Env env; auto [model, x, y] = make_model(env); auto state = std::make_shared<State>();
    auto optimizer = make_optimizer(); add_benders_callback<LazyCutCallback>(optimizer, env, x, y);
    model.use(optimizer); model.optimize();
    CHECK(model.get_status() == Optimal); CHECK(model.get_best_obj() == 5.4_a);
}

TEST_CASE("Universal callback terminates optimization", "[Callbacks]") {
    Env env; auto [model, x, y] = make_model(env); auto state = std::make_shared<State>();
    auto optimizer = make_optimizer(); optimizer.add_callback(Factory(state, x, y, Action::Terminate));
    model.use(optimizer); model.optimize(); CHECK(state->acted);
}
