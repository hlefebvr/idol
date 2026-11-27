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
struct HeuristicState { bool submitted = false, accepted = false; };

class CountingLazyCutCallback : public CallbackFactory {
    std::unique_ptr<Model> separation;
    std::shared_ptr<std::vector<double>> generated_rhs;
    GenerationPattern<Ctr> cut;
    CtrType type;
public:
    CountingLazyCutCallback(const Model& separation_,
                            std::shared_ptr<std::vector<double>> generated_rhs_,
                            GenerationPattern<Ctr> cut_,
                            CtrType type_)
        : separation(separation_.clone()), generated_rhs(std::move(generated_rhs_)), cut(std::move(cut_)), type(type_) {}

    CountingLazyCutCallback(const CountingLazyCutCallback& other)
        : separation(other.separation->clone()), generated_rhs(other.generated_rhs), cut(other.cut), type(other.type) {}

    class Strategy : public impl::CutSeparation {
        std::shared_ptr<std::vector<double>> generated_rhs;
    protected:
        void hook_add_cut(const TempCtr& cut_) override {
            generated_rhs->emplace_back(cut_.rhs());
            add_lazy_cut(cut_);
        }

        [[nodiscard]] std::string name() const override { return "counting-lazy-cut-callback"; }
    public:
        Strategy(Model* separation_,
                 std::shared_ptr<std::vector<double>> generated_rhs_,
                 GenerationPattern<Ctr> cut_,
                 CtrType type_)
            : impl::CutSeparation(IncumbentSolution, separation_, std::move(cut_), type_),
              generated_rhs(std::move(generated_rhs_)) {}
    };

    Callback* operator()() override {
        auto* model = separation->clone();
        model->use(OPTIMIZER().with_max_n_solution_in_pool(2));
        return new Strategy(model, generated_rhs, cut, type);
    }

    CallbackFactory* clone() const override { return new CountingLazyCutCallback(*this); }
};

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

class HeuristicCallback : public Callback {
    std::shared_ptr<HeuristicState> state; std::vector<Var> vars; std::vector<double> values; double objective;
public:
    HeuristicCallback(std::shared_ptr<HeuristicState> s, std::vector<Var> vars_, std::vector<double> values_, double objective_)
        : state(std::move(s)), vars(std::move(vars_)), values(std::move(values_)), objective(objective_) {}
    void operator()(CallbackEvent event) override {
        if (event == InvalidSolution && !state->submitted) {
            PrimalPoint point; for (unsigned i = 0; i < vars.size(); ++i) point.set(vars[i], values[i]); point.set_objective_value(objective);
            submit_heuristic_solution(point); state->submitted = true;
        }
        if (event == IncumbentSolution) {
            const auto point = primal_solution(); bool matches = true;
            for (unsigned i = 0; i < vars.size(); ++i) matches &= std::abs(point.get(vars[i]) - values[i]) <= 1e-7;
            if (!matches) return;
            state->accepted = true;
        }
    }
};

class HeuristicFactory : public CallbackFactory {
    std::shared_ptr<HeuristicState> state; std::vector<Var> vars; std::vector<double> values; double objective;
public:
    HeuristicFactory(std::shared_ptr<HeuristicState> s, std::vector<Var> vars_, std::vector<double> values_, double objective_)
        : state(std::move(s)), vars(std::move(vars_)), values(std::move(values_)), objective(objective_) {}
    Callback* operator()() override { return new HeuristicCallback(state, vars, values, objective); }
    CallbackFactory* clone() const override { return new HeuristicFactory(*this); }
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

TEST_CASE("Lazy-cut separation handles constraint senses and feasibility tolerance", "[Callbacks]") {
    Env env;
    const double tolerance = env.get_tol_feasibility();
    Model model(env);
    const auto x = model.add_var(0, 1, Binary, 1, "x");
    Model separation(env);

    const auto generated_rhs = std::make_shared<std::vector<double>>();
    CtrType type;
    GenerationPattern<Ctr> cut;
    double expected_objective;

    SECTION(">= violated") {
        const auto u = separation.add_var(.5, .5, Continuous, 0, "u");
        cut = x - !u;
        type = GreaterOrEqual;
        expected_objective = 1.;
    }

    SECTION("equivalent sign-reversed <= violated") {
        const auto u = separation.add_var(.5, .5, Continuous, 0, "u");
        cut = -1. * (x - !u);
        type = LessOrEqual;
        expected_objective = 1.;
    }

    SECTION(">= violation within tolerance") {
        const auto u = separation.add_var(.5 * tolerance, .5 * tolerance, Continuous, 0, "u");
        cut = x - !u;
        type = GreaterOrEqual;
        expected_objective = 0.;
    }

    SECTION("equivalent sign-reversed <= violation within tolerance") {
        const auto u = separation.add_var(.5 * tolerance, .5 * tolerance, Continuous, 0, "u");
        cut = -1. * (x - !u);
        type = LessOrEqual;
        expected_objective = 0.;
    }

    SECTION(">= satisfied") {
        const auto u = separation.add_var(-.5, -.5, Continuous, 0, "u");
        cut = x - !u;
        type = GreaterOrEqual;
        expected_objective = 0.;
    }

    SECTION("equivalent sign-reversed <= satisfied") {
        const auto u = separation.add_var(-.5, -.5, Continuous, 0, "u");
        cut = -1. * (x - !u);
        type = LessOrEqual;
        expected_objective = 0.;
    }

    SECTION(">= solution pool contains a nonviolating generator") {
        const auto u = separation.add_var(0, 1, Binary, 0, "u");
        cut = x - !u;
        type = GreaterOrEqual;
        expected_objective = 1.;
    }

    auto optimizer = make_optimizer();
    optimizer.add_callback(CountingLazyCutCallback(separation, generated_rhs, cut, type));
    model.use(optimizer);
    model.optimize();

    CHECK(model.get_status() == Optimal);
    CHECK(model.get_best_obj() == Catch::Approx(expected_objective));
    CHECK((!generated_rhs->empty()) == (expected_objective == 1.));
    for (const double rhs : *generated_rhs) {
        CHECK(std::abs(rhs) > tolerance);
    }
}

TEST_CASE("Lazy-cut separation rejects equality constraints explicitly", "[Callbacks]") {
    Env env;
    Model model(env);
    const auto x = model.add_var(0, 1, Binary, 1, "x");
    Model separation(env);
    const auto u = separation.add_var(1, 1, Continuous, 0, "u");
    const auto generated_rhs = std::make_shared<std::vector<double>>();
    auto optimizer = make_optimizer();
    optimizer.add_callback(CountingLazyCutCallback(separation, generated_rhs, x - !u, Equal));
    CHECK_THROWS_WITH(([&] { model.use(optimizer); model.optimize(); }()),
                      "Separating equality constraints is not available.");
}

TEST_CASE("Universal callback terminates optimization", "[Callbacks]") {
    Env env; auto [model, x, y] = make_model(env); auto state = std::make_shared<State>();
    auto optimizer = make_optimizer(); optimizer.add_callback(Factory(state, x, y, Action::Terminate));
    model.use(optimizer); model.optimize(); CHECK(state->acted);
}

TEST_CASE("Universal callback submits a heuristic solution", "[Callbacks]") {
    Env env; auto [model, y, z] = make_model(env); auto state = std::make_shared<HeuristicState>(); auto optimizer = make_optimizer();
    add_benders_callback<LazyCutCallback>(optimizer, env, y, z);
    optimizer.add_callback(HeuristicFactory(state, {y, z}, {2, 1.4}, 5.4)); model.use(optimizer); model.optimize();
    CHECK(state->submitted); CHECK(model.get_status() == Optimal); CHECK(model.get_best_obj() == 5.4_a);
}
