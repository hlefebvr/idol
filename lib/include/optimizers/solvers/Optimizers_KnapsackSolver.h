//
// Created by henri on 05/04/23.
//

#ifndef IDOL_OPTIMIZERS_KNAPSACKSOLVER_H
#define IDOL_OPTIMIZERS_KNAPSACKSOLVER_H

#ifdef IDOL_USE_KP_SOLVER

#include "OptimizerWithLazyUpdates.h"
#define NDEBUG // Turns off logs for knapsacksolver
#include "knapsacksolver/instance.hpp"

namespace Optimizers {
    class KnapsackSolver;
}

class Optimizers::KnapsackSolver : public Optimizer {
    std::unique_ptr<knapsacksolver::Solution> m_solution;

    double m_max_min_factor = 1.;
    int m_int_double_factor = 1e4;

    knapsacksolver::Instance create_instance();
public:
    explicit KnapsackSolver(const Model& t_model);

    [[nodiscard]] std::string name() const override;

protected:
    void build() override;

    void add(const Var &t_var) override;

    void add(const Ctr &t_ctr) override;

    void remove(const Var &t_var) override;

    void remove(const Ctr &t_ctr) override;

    void update() override;

    void write(const std::string &t_name) override;

    void hook_optimize() override;

    void set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) override;

    [[nodiscard]] double get(const Req<double, void> &t_attr) const override;

    [[nodiscard]] int get(const Req<int, void> &t_attr) const override;

    [[nodiscard]] double get(const Req<double, Var> &t_attr, const Var& t_var) const override;
};

#endif

#endif //IDOL_OPTIMIZERS_KNAPSACKSOLVER_H
