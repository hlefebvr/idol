//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITION_H
#define OPTIMIZE_DECOMPOSITION_H

#include "GenerationAlgorithm.h"
#include <functional>
#include <memory>

class Decomposition : public Algorithm {
    std::unique_ptr<Algorithm> m_rmp_strategy;
    std::list<std::unique_ptr<Algorithm>> m_generation_strategies;
protected:
    bool set_parameter_double(const Parameter<double> &t_param, double t_value) override;

    bool set_parameter_int(const Parameter<int> &t_param, int t_value) override;

    bool set_parameter_bool(const Parameter<bool> &t_param, bool t_value) override;

    [[nodiscard]] std::optional<double> get_parameter_double(const Parameter<double> &t_param) const override;

    [[nodiscard]] std::optional<int> get_parameter_int(const Parameter<int> &t_param) const override;

    void execute() override;
public:
    Decomposition() = default;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    void update_var_lb(const Var &t_var, double t_lb) override;

    void update_var_ub(const Var &t_var, double t_ub) override;

    void update_rhs_coeff(const Ctr &t_ctr, double t_rhs) override;

    Ctr add_ctr(TempCtr&& t_temporary_constraint) override;

    void remove(const Ctr &t_constraint) override;

    double get_lb(const Var &t_var) const override;

    double get_ub(const Var &t_var) const override;

    VarType get_type(const Var &t_var) const override;

    const Column &get_column(const Var &t_var) const override;

    bool has(const Var &t_var) const override;

    bool has(const Ctr &t_ctr) const override;

    const Row &get_row(const Ctr &t_ctr) const override;

    CtrType get_type(const Ctr &t_ctr) const override;

    template<class T, class ...Args> T& set_rmp_solution_strategy(Args&& ...t_args) {
        auto* solution_strategy = new T(std::forward<Args>(t_args)...);
        m_rmp_strategy.reset(solution_strategy);
        m_rmp_strategy->set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);
        return *solution_strategy;
    }

    template<class T, class ...Args> T& add_generation_strategy(Args&& ...t_args) {

        auto* rmp_strategy = m_generation_strategies.empty() ? m_rmp_strategy.get() : m_generation_strategies.back().get();

        auto* generation_strategy = new T(*rmp_strategy, std::forward<Args>(t_args)...);

        m_generation_strategies.emplace_back(generation_strategy);

        return *generation_strategy;
    }
};

#endif //OPTIMIZE_DECOMPOSITION_H
