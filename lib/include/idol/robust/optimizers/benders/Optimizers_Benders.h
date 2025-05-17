//
// Created by henri on 08.05.25.
//

#ifndef IDOL_OPTIMIZERS_ROBUST_BENDERS_H
#define IDOL_OPTIMIZERS_ROBUST_BENDERS_H

#include "idol/general/optimizers/Algorithm.h"
#include "idol/robust/modeling/Description.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Formulation.h"
#include "idol/mixed-integer/optimizers/callbacks/CallbackFactory.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"

namespace idol::Optimizers::Robust {
    class Benders;
}

class idol::Optimizers::Robust::Benders : public Algorithm {
    const ::idol::Robust::Description &m_robust_description;
    const ::idol::Bilevel::Description &m_bilevel_description;
    std::unique_ptr<idol::CCG::Formulation> m_formulation;

    // Initial scenarios
    std::vector<Point<Var>> m_initial_scenarios;

    // Feasibility Separation
    std::vector<std::unique_ptr<OptimizerFactory>> m_optimizer_feasibility_separation;

    // Optimality Separation
    std::vector<std::unique_ptr<OptimizerFactory>> m_optimizer_optimality_separation;

    // Joint Separation
    std::vector<std::unique_ptr<OptimizerFactory>> m_optimizer_joint_separation;
public:
    Benders(const Model& t_parent,
            const ::idol::Robust::Description &t_robust_description,
            const ::idol::Bilevel::Description &t_bilevel_description,
            std::vector<Point<Var>> t_initial_scenarios,
            const std::list<std::unique_ptr<OptimizerFactory>>& t_optimizer_feasibility_separation,
            const std::list<std::unique_ptr<OptimizerFactory>>& t_optimizer_optimality_separation,
            const std::list<std::unique_ptr<OptimizerFactory>>& t_optimizer_joint_separation);

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;

protected:
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void add(const QCtr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void remove(const QCtr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;
    void hook_before_optimize() override;
    void hook_optimize() override;
    void set_solution_index(unsigned int t_index) override;
    void update_obj_sense() override;
    void update_obj() override;
    void update_rhs() override;
    void update_obj_constant() override;
    void update_mat_coeff(const Ctr &t_ctr, const Var &t_var) override;
    void update_ctr_type(const Ctr &t_ctr) override;
    void update_ctr_rhs(const Ctr &t_ctr) override;
    void update_var_type(const Var &t_var) override;
    void update_var_lb(const Var &t_var) override;
    void update_var_ub(const Var &t_var) override;
    void update_var_obj(const Var &t_var) override;

    class BendersCallback : public ::idol::CallbackFactory {
        const Benders &m_parent;
    public:
        explicit BendersCallback(const Benders& t_parent);
        Callback *operator()() override;
        [[nodiscard]] CallbackFactory *clone() const override;

        class Strategy : public ::idol::Callback {
            const Benders &m_parent;
            std::list<Point<Var>> m_scenarios;
        public:
            explicit Strategy(const Benders& t_parent);

        protected:
            void operator()(CallbackEvent t_event) override;
        };

    };
};

#endif //IDOL_OPTIMIZERS_ROBUST_BENDERS_H
