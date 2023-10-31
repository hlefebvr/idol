//
// Created by henri on 24/03/23.
//

#ifndef IDOL_ARCHIVEDDANTZIGWOLFEDECOMPOSITION_H
#define IDOL_ARCHIVEDDANTZIGWOLFEDECOMPOSITION_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/constraints/Ctr.h"
#include "idol/optimizers/archive/column-generation/ArchivedColumnGeneration.h"

namespace idol {
    class ArchivedDantzigWolfeDecomposition;
}

class idol::ArchivedDantzigWolfeDecomposition : public impl::OptimizerFactoryWithColumnGenerationParameters<ArchivedDantzigWolfeDecomposition> {
    // Basic parameters
    Annotation<Ctr, unsigned int> m_decomposition;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<OptimizerFactory> m_pricing_optimizer;

    // Branching parameters
    std::optional<bool> m_branching_on_master;

    std::optional<CtrType> m_aggregation_type;

    ArchivedDantzigWolfeDecomposition(const ArchivedDantzigWolfeDecomposition& t_src);

    Annotation<Var, unsigned int> create_variable_flag(const Model& t_model, unsigned int *t_n_subproblem) const;
    std::vector<Model*> create_empty_subproblems(Env& t_env, unsigned int t_n_subproblems) const;
    [[nodiscard]] std::vector<Column> create_empty_generation_patterns(unsigned int t_n_subproblems) const;
    void dispatch_variables(const Annotation<Var, unsigned int>& t_variable_flag,
                            const Model& t_original_formulation,
                            Model* t_master,
                            const std::vector<Model*>& t_subproblems) const;
    void dispatch_constraints(const Annotation<Var, unsigned int>& t_variable_flag,
                            const Model& t_original_formulation,
                            Model* t_master,
                            const std::vector<Model*>& t_subproblems,
                            std::vector<Column>& t_generation_patterns) const;
    void dispatch_linking_constraint(const Annotation<Var, unsigned int>& t_variable_flag,
                                     const Ctr& t_ctr,
                                     const Row& t_row,
                                     CtrType t_type,
                                     Model* t_master,
                                     const std::vector<Model*>& t_subproblems,
                                     std::vector<Column>& t_generation_patterns) const;
    void dispatch_objective(const Annotation<Var, unsigned int>& t_variable_flag,
                       const Model& t_original_formulation,
                       Model* t_master,
                       std::vector<Column>& t_generation_patterns) const;
    std::pair<Expr<Var, Var>, std::vector<Constant>> dispatch_linking_expression(
                                    const Annotation<Var, unsigned int>& t_variable_flag,
                                    const QuadExpr<Var, Var>& t_qaud,
                                    const LinExpr<Var>& t_lin,
                                    Model* t_master,
                                    unsigned int t_n_subproblems) const;
    void add_convexity_constraints(Env& t_env, Model* t_master, std::vector<Column>& t_generation_patterns) const;
public:
    explicit ArchivedDantzigWolfeDecomposition(const Annotation<Ctr, unsigned int>& t_decomposition);

    /**
     * Configures the optimizer factory for the master problem
     * @param t_master_optimizer the desired optimizer factory for the master problem
     * @return the optimizer factory itself
     */
    ArchivedDantzigWolfeDecomposition& with_master_optimizer(const OptimizerFactory& t_master_optimizer);

    /**
     * Configures the optimizer factory for the pricing problems
     * @param t_pricing_optimizer the desired optimizer factory for the pricing problems
     * @return the optimizer factory itself
     */
    ArchivedDantzigWolfeDecomposition& with_pricing_optimizer(const OptimizerFactory& t_pricing_optimizer);

    /**
     * When set to true, branching is applied to the master problem. Otherwise, branching decisions are applied to the
     * pricing problems.
     * @param t_value the desired value
     * @return the optimizer factory itself
     */
    ArchivedDantzigWolfeDecomposition& with_branching_on_master(bool t_value);

    ArchivedDantzigWolfeDecomposition& with_aggregation_type(CtrType t_type);

    Optimizer *operator()(const Model &t_original_formulation) const override;

    [[nodiscard]] ArchivedDantzigWolfeDecomposition *clone() const override;
};

#endif //IDOL_ARCHIVEDDANTZIGWOLFEDECOMPOSITION_H
