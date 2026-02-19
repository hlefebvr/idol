//
// Created by Henri on 03/02/2026.
//

#ifndef IDOL_OPTIMIZERS_NESTEDBRANCHANDCUT_H
#define IDOL_OPTIMIZERS_NESTEDBRANCHANDCUT_H

#include "Formulation.h"
#include "idol/robust/modeling/Description.h"
#include "idol/general/optimizers/Algorithm.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"

namespace idol::Optimizers::Robust {
    class BilevelBasedBranchAndBound;
}

class idol::Optimizers::Robust::BilevelBasedBranchAndBound : public Algorithm {
    class Formulation;

    const ::idol::Robust::Description &m_robust_description;
    const ::idol::Bilevel::Description &m_bilevel_description;

    std::unique_ptr<OptimizerFactory> m_optimality_bilevel_optimizer;
    std::unique_ptr<OptimizerFactory> m_feasibility_bilevel_optimizer;
    bool m_with_first_stage_relaxation;

    std::unique_ptr<idol::Robust::BBBB::Formulation> m_formulation;
public:
    BilevelBasedBranchAndBound(const Model& t_model,
                       const idol::Robust::Description& t_robust_description,
                       const ::idol::Bilevel::Description& t_bilevel_description,
                       const OptimizerFactory& t_optimality_optimizer,
                       const OptimizerFactory& t_feasibility_optimizer,
                       bool t_with_first_stage_relaxation
    );

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] double get_var_primal(const Var& t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var& t_var) const override;
    [[nodiscard]] double get_var_ray(const Var& t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const override;
    [[nodiscard]] unsigned get_n_solutions() const override;
    [[nodiscard]] unsigned get_solution_index() const override;

    idol::Robust::BBBB::Formulation& get_formulation();
    [[nodiscard]] const idol::Robust::BBBB::Formulation& get_formulation() const;
    [[nodiscard]] const idol::Robust::Description& get_robust_description() const { return m_robust_description; }
    [[nodiscard]] const idol::Bilevel::Description& get_bilevel_description() const { return m_bilevel_description; }
    [[nodiscard]] const idol::OptimizerFactory& get_feasibility_bilevel_optimizer() const { return *m_feasibility_bilevel_optimizer; }
    [[nodiscard]] const idol::OptimizerFactory& get_optimality_bilevel_optimizer() const { return *m_optimality_bilevel_optimizer; }
    [[nodiscard]] bool relax_first_stage_decisions() const { return m_with_first_stage_relaxation; }

protected:
    void add(const Var& t_var) override;
    void add(const Ctr& t_ctr) override;
    void add(const QCtr& t_ctr) override;
    void remove(const Var& t_var) override;
    void remove(const Ctr& t_ctr) override;
    void remove(const QCtr& t_ctr) override;
    void update() override;
    void write(const std::string& t_name) override;
    void hook_before_optimize() override;
    void hook_optimize() override;
    void hook_after_optimize() override;
    void set_solution_index(unsigned t_index) override;
    void update_obj_sense() override;
    void update_obj() override;
    void update_rhs() override;
    void update_obj_constant() override;
    void update_mat_coeff(const Ctr& t_ctr, const Var& t_var) override;
    void update_ctr_type(const Ctr& t_ctr) override;
    void update_ctr_rhs(const Ctr& t_ctr) override;
    void update_var_type(const Var& t_var) override;
    void update_var_lb(const Var& t_var) override;
    void update_var_ub(const Var& t_var) override;
    void update_var_obj(const Var& t_var) override;

    class Node : public idol::DefaultNodeInfo {
        BilevelBasedBranchAndBound* m_parent;
    public:
        Node() = delete;
        Node(BilevelBasedBranchAndBound* t_parent) : m_parent(t_parent) {}
        void save(const Model& t_original_formulation, const Model& t_model) override;
        [[nodiscard]] Node* create_child() const override;
        [[nodiscard]] Node* clone() const override;

        static DefaultNodeUpdator<Node>* create_updator(const Model& t_src_model, Model& t_relaxation);
    };
};

#endif //IDOL_OPTIMIZERS_NESTEDBRANCHANDCUT_H