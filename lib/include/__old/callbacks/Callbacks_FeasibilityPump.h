//
// Created by henri on 21/12/22.
//

#ifndef IDOL_CALLBACKS_FEASIBILITYPUMP_H
#define IDOL_CALLBACKS_FEASIBILITYPUMP_H

#include <random>
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/BranchAndBound_Events.h"
#include "UserCallback.h"

namespace Callbacks {
    class FeasibilityPump;
}

IDOL_CREATE_PARAMETER_CLASS(FeasibilityPump);

IDOL_CREATE_PARAMETER_TYPE(FeasibilityPump, int, 3)

IDOL_CREATE_PARAMETER(FeasibilityPump, int, 0, CallFrequency, 10)
IDOL_CREATE_PARAMETER(FeasibilityPump, int, 1, MaxLevel, 10)
IDOL_CREATE_PARAMETER(FeasibilityPump, int, 2, NumberOfVariableFlip, 10)

class Callbacks::FeasibilityPump : public UserCallback<BranchAndBound> {
    Param::FeasibilityPump::values<int> m_int_parameters;

    std::list<Var> m_binary_variables;
    std::list<Var> m_integer_variables;

    std::random_device m_device;
    std::mt19937 m_generator;
public:
    explicit FeasibilityPump(const Model& t_model) : m_generator(m_device()) {

        for (const auto& var : t_model.vars()) {
            if (const auto type = t_model.get(Attr::Var::Type, var) ; type == Binary) {
                m_binary_variables.emplace_back(var);
            } else if (type == Integer) {
                m_integer_variables.emplace_back(var);
            }
        }

        if (!m_integer_variables.empty()) {
            throw NotImplemented("Applying feasibility pump with integer variables", "Callbacks::FeasibilityPump::execute");
        }

    }

    void set(const Parameter<int> &t_param, int t_value) override {

        if (t_param.is_in_section(Param::Sections::FeasibilityPump)) {
            m_int_parameters.set(t_param, t_value);
        }

        AbstractCallback::set(t_param, t_value);
    }

    int get(const Parameter<int> &t_param) const override {

        if (t_param.is_in_section(Param::Sections::FeasibilityPump)) {
            return m_int_parameters.get(t_param);
        }

        return AbstractCallback::get(t_param);
    }

    Solution::Primal round(const Solution::Primal& t_primals) {
        auto result = t_primals;

        for (const auto& var : m_binary_variables) {
            result.set(var, std::round( t_primals.get(var) ));
        }

        return result;
    }

    Expr<Var, Var> Delta(const Solution::Primal& t_rounded) {
        Expr<Var, Var> result;

        for (const auto& var : m_binary_variables) {
            const double value = t_rounded.get(var);
            if (value < .5) {
                result += var;
            } else {
                result += 1 - var;
            }
        }

        return result;
    }

    bool not_equal(const Solution::Primal& t_a, const Solution::Primal& t_b) {

        for (const auto& var : m_binary_variables) {
            if (!equals(t_a.get(var), t_b.get(var), ToleranceForIntegrality)) {
                return true;
            }
        }

        return false;
    }

    bool is_integer(const Solution::Primal& t_x_star) {

        for (const auto& var : m_binary_variables) {
            const double value = t_x_star.get(var);
            if (!equals(std::round(value), value, ToleranceForIntegrality)) {
                return false;
            }
        }

        return true;

    }

    std::vector<std::pair<double, Var>> get_to_flip(unsigned int t_n, const Solution::Primal& t_x_star, const Solution::Primal& t_x_tilde) {
        std::vector<std::pair<double, Var>> result;

        for (const auto& var : m_binary_variables) {
            result.emplace_back( std::abs( t_x_star.get(var) - t_x_tilde.get(var) ), var );
        }

        std::sort(result.begin(), result.end(), [](const auto& t_a, const auto& t_b){
            return t_a.first > t_b.first;
        });

        result.erase(result.begin() + t_n, result.end());

        return result;
    }

    void execute(const EventType& t_event) override {

        if (t_event != Event_::BranchAndBound::RelaxationSolved) {
            return;
        }

        const auto level = parent().current_node().level();

        if (level > get(Param::FeasibilityPump::MaxLevel) || level % get(Param::FeasibilityPump::CallFrequency) != 0) {
            return;
        }

        idol_Log(Trace, FeasibilityPump, "Begin feasibility pump heuristic.");

        const auto number_of_flips = get(Param::FeasibilityPump::NumberOfVariableFlip);
        std::uniform_int_distribution<unsigned int> distribution(number_of_flips / 2, 3 * number_of_flips / 2);

        auto& solver = parent().solution_strategy();

        const auto objective = solver.get(Attr::Obj::Expr);

        Solution::Primal x_star = parent().current_node().primal_solution();
        Solution::Primal x_tilde = round(x_star);

        unsigned int iter = 0;
        do {

            solver.set(Attr::Obj::Expr, Delta(x_tilde));
            solver.solve();

            x_star = solver.primal_solution();

            if (is_integer(x_star)) {
                break;
            }

            auto x_star_rounded = round(x_star);

            if (not_equal(x_star_rounded, x_tilde)) {
                x_tilde = std::move(x_star_rounded);
            } else {
                auto to_flip = get_to_flip(distribution(m_generator), x_star, x_tilde);

                for (const auto& [score, var] : to_flip) {
                    x_tilde.set(var, 1 - x_tilde.get(var));
                }

            }

            ++iter;
        } while (iter < 10);

        double obj_val = objective.constant().numerical();
        for (const auto& [var, value] : x_star) {
            obj_val += objective.linear().get(var).numerical() * value;
        }

        x_star.set_objective_value(obj_val);

        solver.set(Attr::Obj::Expr, std::move(objective));

        parent().submit_solution(std::move(x_star));

        idol_Log(Trace, FeasibilityPump, "End feasibility pump heuristic.");

    }
};

#endif //IDOL_CALLBACKS_FEASIBILITYPUMP_H
