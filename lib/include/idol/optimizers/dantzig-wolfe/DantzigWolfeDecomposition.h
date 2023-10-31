//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFEDECOMPOSITION_H
#define IDOL_DANTZIGWOLFEDECOMPOSITION_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "DantzigWolfeSubProblem.h"
#include "idol/containers/Map.h"
#include "DantzigWolfeFormulation.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/DualPriceSmoothingStabilization.h"

namespace idol::DantzigWolfe {
    class Decomposition;
}

class idol::DantzigWolfe::Decomposition : public OptimizerFactoryWithDefaultParameters<Decomposition> {
    Annotation<Ctr, unsigned int> m_decomposition;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::unique_ptr<InfeasibilityStrategyFactory> m_infeasibility_strategy;
    std::unique_ptr<DualPriceSmoothingStabilization> m_dual_price_smoothing_stabilization;
    std::optional<unsigned int> m_max_parallel_sub_problems;
    std::optional<bool> m_use_hard_branching;
    std::optional<SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, SubProblem> m_sub_problem_specs;

    Decomposition(const Decomposition& t_src);
    std::vector<SubProblem> create_sub_problems_specifications(const Formulation &t_dantzig_wolfe_formulation) const;
    static void add_aggregation_constraints(Formulation& t_dantzig_wolfe_formulation, const std::vector<SubProblem>& t_sub_problem_specifications) ;
public:
    explicit Decomposition(Annotation<Ctr, unsigned int> t_decomposition);

    Optimizer *operator()(const Model &t_model) const override;

    OptimizerFactory *clone() const override;

    Decomposition& with_master_optimizer(const OptimizerFactory& t_optimizer_factory);

    Decomposition& with_default_sub_problem_spec(SubProblem t_sub_problem);

    Decomposition& with_sub_problem_spec(unsigned int t_id, SubProblem t_sub_problem);

    Decomposition& with_infeasibility_strategy(const InfeasibilityStrategyFactory& t_strategy);

    Decomposition& with_hard_branching(bool t_value);

    Decomposition& with_max_parallel_sub_problems(unsigned int t_n_sub_problems);

    Decomposition& with_dual_price_smoothing_stabilization(const DualPriceSmoothingStabilization& t_stabilization);

    const SubProblem& get_sub_problem_spec(unsigned int t_id) const;
};

#endif //IDOL_DANTZIGWOLFEDECOMPOSITION_H
