//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFEDECOMPOSITION_H
#define IDOL_DANTZIGWOLFEDECOMPOSITION_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "SubProblem.h"
#include "idol/general/utils/Map.h"
#include "Formulation.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/DualPriceSmoothingStabilization.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/logs/Factory.h"

namespace idol {
    class DantzigWolfeDecomposition;
}

class idol::DantzigWolfeDecomposition : public OptimizerFactoryWithDefaultParameters<DantzigWolfeDecomposition> {
    std::optional<Annotation<unsigned int>> m_decomposition;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::unique_ptr<DantzigWolfe::InfeasibilityStrategyFactory> m_infeasibility_strategy;
    std::unique_ptr<DantzigWolfe::DualPriceSmoothingStabilization> m_dual_price_smoothing_stabilization;
    std::unique_ptr<Logs::DantzigWolfe::Factory> m_logger_factory;
    std::optional<unsigned int> m_max_parallel_sub_problems;
    std::optional<bool> m_use_hard_branching;
    std::optional<bool> m_use_infeasible_column_removal_when_branching;
    std::optional<DantzigWolfe::SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, DantzigWolfe::SubProblem> m_sub_problem_specs;

    [[nodiscard]] std::vector<DantzigWolfe::SubProblem> create_sub_problems_specifications(const DantzigWolfe::Formulation &t_dantzig_wolfe_formulation) const;
    static void add_aggregation_constraints(DantzigWolfe::Formulation& t_dantzig_wolfe_formulation, const std::vector<DantzigWolfe::SubProblem>& t_sub_problem_specifications) ;
public:
    DantzigWolfeDecomposition() = default;

    explicit DantzigWolfeDecomposition(Annotation<unsigned int> t_decomposition);

    DantzigWolfeDecomposition(const DantzigWolfeDecomposition& t_src);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] DantzigWolfeDecomposition *clone() const override;

    DantzigWolfeDecomposition& with_master_optimizer(const OptimizerFactory& t_optimizer_factory);

    DantzigWolfeDecomposition& with_default_sub_problem_spec(DantzigWolfe::SubProblem t_sub_problem);

    DantzigWolfeDecomposition& with_sub_problem_spec(unsigned int t_id, DantzigWolfe::SubProblem t_sub_problem);

    DantzigWolfeDecomposition& with_infeasibility_strategy(const DantzigWolfe::InfeasibilityStrategyFactory& t_strategy);

    DantzigWolfeDecomposition& with_hard_branching(bool t_value);

    DantzigWolfeDecomposition& with_infeasible_columns_removal(bool t_value);

    DantzigWolfeDecomposition& with_max_parallel_sub_problems(unsigned int t_n_sub_problems);

    DantzigWolfeDecomposition& with_logger(const Logs::DantzigWolfe::Factory& t_logger);

    DantzigWolfeDecomposition& with_dual_price_smoothing_stabilization(const DantzigWolfe::DualPriceSmoothingStabilization& t_stabilization);

    [[nodiscard]] const DantzigWolfe::SubProblem& get_sub_problem_spec(unsigned int t_id) const;
};

#endif //IDOL_DANTZIGWOLFEDECOMPOSITION_H
