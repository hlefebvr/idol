//
// Created by henri on 31.10.23.
//
#include <memory>

#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/Formulation.h"
#include "idol/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/NoStabilization.h"
#include "idol/optimizers/dantzig-wolfe/logs/Debug.h"

idol::OptimizerFactory *idol::DantzigWolfeDecomposition::clone() const {
    return new DantzigWolfeDecomposition(*this);
}

idol::DantzigWolfeDecomposition::DantzigWolfeDecomposition(idol::Annotation<idol::Ctr, unsigned int> t_decomposition)
    : m_decomposition(std::move(t_decomposition)) {}

idol::DantzigWolfeDecomposition::DantzigWolfeDecomposition(const DantzigWolfeDecomposition& t_src)
    : OptimizerFactoryWithDefaultParameters<DantzigWolfeDecomposition>(t_src),
      m_decomposition(t_src.m_decomposition),
      m_master_optimizer_factory(t_src.m_master_optimizer_factory ? t_src.m_master_optimizer_factory->clone() : nullptr),
      m_dual_price_smoothing_stabilization(t_src.m_dual_price_smoothing_stabilization ? t_src.m_dual_price_smoothing_stabilization->clone() : nullptr),
      m_max_parallel_sub_problems(t_src.m_max_parallel_sub_problems),
      m_use_hard_branching(t_src.m_use_hard_branching),
      m_use_infeasible_column_removal_when_branching(t_src.m_use_infeasible_column_removal_when_branching),
      m_infeasibility_strategy(t_src.m_infeasibility_strategy ? t_src.m_infeasibility_strategy->clone() : nullptr),
      m_default_sub_problem_spec(t_src.m_default_sub_problem_spec),
      m_sub_problem_specs(t_src.m_sub_problem_specs),
      m_logger_factory(t_src.m_logger_factory ? t_src.m_logger_factory->clone() : nullptr)
{}

idol::Optimizer *idol::DantzigWolfeDecomposition::operator()(const Model &t_model) const {

    if (!m_master_optimizer_factory) {
        throw Exception("No optimizer for master has been configured.");
    }

    DantzigWolfe::Formulation dantzig_wolfe_formulation(t_model, m_decomposition);

    auto sub_problems_specifications = create_sub_problems_specifications(dantzig_wolfe_formulation);

    add_aggregation_constraints(dantzig_wolfe_formulation, sub_problems_specifications);

    std::unique_ptr<DantzigWolfe::InfeasibilityStrategyFactory> default_strategy;
    if (!m_infeasibility_strategy) {
        default_strategy = std::make_unique<DantzigWolfe::FarkasPricing>();
    }

    std::unique_ptr<DantzigWolfe::DualPriceSmoothingStabilization> dual_price_smoothing;
    if (!m_dual_price_smoothing_stabilization) {
        dual_price_smoothing = std::make_unique<DantzigWolfe::NoStabilization>();
    }

    std::unique_ptr<DantzigWolfe::LoggerFactory> logger_factory;
    if (!m_logger_factory) {
        logger_factory = std::make_unique<DantzigWolfe::Loggers::Debug>();
    }

    const bool use_hard_branching = m_use_hard_branching.has_value() && m_use_hard_branching.value();
    const bool remove_infeasible_column = m_use_infeasible_column_removal_when_branching.has_value() ? m_use_infeasible_column_removal_when_branching.value() : use_hard_branching;

    return new Optimizers::DantzigWolfeDecomposition(t_model,
                                                     std::move(dantzig_wolfe_formulation),
                                                     *m_master_optimizer_factory,
                                                     m_dual_price_smoothing_stabilization ? *m_dual_price_smoothing_stabilization : *dual_price_smoothing,
                                                     m_max_parallel_sub_problems.has_value() ? m_max_parallel_sub_problems.value() : 1,
                                                     use_hard_branching,
                                                     remove_infeasible_column,
                                                     std::move(sub_problems_specifications),
                                                     m_infeasibility_strategy ? *m_infeasibility_strategy : *default_strategy,
                                                     *logger_factory
                                                     );
}

std::vector<idol::DantzigWolfe::SubProblem> idol::DantzigWolfeDecomposition::create_sub_problems_specifications(
        const DantzigWolfe::Formulation &t_dantzig_wolfe_formulation) const {

    const auto n_sub_problems = t_dantzig_wolfe_formulation.n_sub_problems();

    std::vector<DantzigWolfe::SubProblem> result;
    result.reserve(n_sub_problems);

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {

        auto spec = get_sub_problem_spec(i);

        if (spec.phases().size() == 0) {
            throw Exception("Sub-problem " + std::to_string(i) + " has no optimizer.");
        }

        result.emplace_back(std::move(spec));
    }

    return result;
}

void idol::DantzigWolfeDecomposition::add_aggregation_constraints(
        DantzigWolfe::Formulation &t_dantzig_wolfe_formulation,
        const std::vector<DantzigWolfe::SubProblem>& t_sub_problem_specifications) {

    const auto n_sub_problems = t_dantzig_wolfe_formulation.n_sub_problems();

    for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {

        const auto& spec = t_sub_problem_specifications[i];
        t_dantzig_wolfe_formulation.add_aggregation_constraint(i,
                                                               spec.lower_multiplicity(),
                                                               spec.upper_multiplicity()
                                                                );

    }

}

const idol::DantzigWolfe::SubProblem &idol::DantzigWolfeDecomposition::get_sub_problem_spec(unsigned int t_id) const {

    auto it = m_sub_problem_specs.find(t_id);
    if (it == m_sub_problem_specs.end()) {

        if (!m_default_sub_problem_spec.has_value()) {
            throw Exception("Sub-problem " + std::to_string(t_id) + " has no specification and "
                                      "no default specification has been configured.");
        }

        return m_default_sub_problem_spec.value();
    }

    return it->second;

}

idol::DantzigWolfeDecomposition &
idol::DantzigWolfeDecomposition::with_master_optimizer(const idol::OptimizerFactory &t_optimizer_factory) {

    if (m_master_optimizer_factory) {
        throw Exception("An optimizer factory has already been given.");
    }

    m_master_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::DantzigWolfeDecomposition &
idol::DantzigWolfeDecomposition::with_default_sub_problem_spec(idol::DantzigWolfe::SubProblem t_sub_problem) {

    if (m_default_sub_problem_spec) {
        throw Exception("A default sub-problem specification has already been given.");
    }

    m_default_sub_problem_spec.emplace(std::move(t_sub_problem));

    return *this;
}

idol::DantzigWolfeDecomposition &
idol::DantzigWolfeDecomposition::with_sub_problem_spec(unsigned int t_id, idol::DantzigWolfe::SubProblem t_sub_problem) {

    auto [it, success] = m_sub_problem_specs.emplace(t_id, std::move(t_sub_problem));

    if (!success) {
        throw Exception("A sub-problem specification with id " + std::to_string(t_id) + " has already been given.");
    }

    return *this;
}

idol::DantzigWolfeDecomposition &idol::DantzigWolfeDecomposition::with_infeasibility_strategy(
        const idol::DantzigWolfe::InfeasibilityStrategyFactory &t_strategy) {

    if (m_infeasibility_strategy) {
        throw Exception("An infeasibility strategy has already been configured.");
    }

    m_infeasibility_strategy.reset(t_strategy.clone());

    return *this;
}

idol::DantzigWolfeDecomposition &idol::DantzigWolfeDecomposition::with_hard_branching(bool t_value) {

    if (m_use_hard_branching.has_value()) {
        throw Exception("Hard branching has already been configured.");
    }

    m_use_hard_branching = t_value;

    return *this;
}

idol::DantzigWolfeDecomposition &
idol::DantzigWolfeDecomposition::with_max_parallel_sub_problems(unsigned int t_n_sub_problems) {

    if (m_max_parallel_sub_problems.has_value()) {
        throw Exception("Maximum number of parallel sub-problems has already been configured.");
    }

    m_max_parallel_sub_problems = t_n_sub_problems;

    return *this;
}

idol::DantzigWolfeDecomposition &idol::DantzigWolfeDecomposition::with_dual_price_smoothing_stabilization(
        const idol::DantzigWolfe::DualPriceSmoothingStabilization &t_stabilization) {

    if (m_dual_price_smoothing_stabilization) {
        throw Exception("A dual price smoothing stabilization has already been configured.");
    }

    m_dual_price_smoothing_stabilization.reset(t_stabilization.clone());

    return *this;
}

idol::DantzigWolfeDecomposition &
idol::DantzigWolfeDecomposition::with_logger(const idol::DantzigWolfe::LoggerFactory &t_logger) {

    if (m_logger_factory) {
        throw Exception("A logger has already been configured.");
    }

    m_logger_factory.reset(t_logger.clone());

    return *this;
}

idol::DantzigWolfeDecomposition &idol::DantzigWolfeDecomposition::with_infeasible_columns_removal(bool t_value) {

    if (m_use_infeasible_column_removal_when_branching.has_value()) {
        throw Exception("Infeasible column removal has already been configured.");
    }

    m_use_infeasible_column_removal_when_branching = t_value;

    return *this;
}

