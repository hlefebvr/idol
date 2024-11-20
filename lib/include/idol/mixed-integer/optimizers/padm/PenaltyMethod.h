//
// Created by henri on 19.09.24.
//

#ifndef IDOL_PENALTYMETHOD_H
#define IDOL_PENALTYMETHOD_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "idol/general/utils/Map.h"
#include "SubProblem.h"
#include "Formulation.h"
#include "PenaltyUpdates.h"
#include "Optimizers_PADM.h"
#include <optional>

namespace idol {
    class PenaltyMethod;
}

class idol::PenaltyMethod : public OptimizerFactoryWithDefaultParameters<PenaltyMethod> {
public:
    explicit PenaltyMethod(Annotation<bool> t_penalized_constraints);

    PenaltyMethod(const PenaltyMethod& t_src);
    PenaltyMethod(PenaltyMethod&&) = default;

    PenaltyMethod& operator=(const PenaltyMethod&) = delete;
    PenaltyMethod& operator=(PenaltyMethod&&) = default;

    PenaltyMethod& with_optimizer(const OptimizerFactory& t_optimizer_factory);

    PenaltyMethod& with_rescaling(bool t_rescaling, double t_threshold);

    PenaltyMethod& with_penalty_update(const PenaltyUpdate& t_penalty_update);

    PenaltyMethod& with_feasible_solution_status(SolutionStatus t_status);

    PenaltyMethod& with_initial_penalty_parameter(double t_value);

    Optimizers::PADM *operator()(const Model &t_model) const override;

    PenaltyMethod& operator+=(const OptimizerFactory& t_optimizer_factory);

    [[nodiscard]] PenaltyMethod *clone() const override;
private:
    Annotation<unsigned int> m_decomposition;
    Annotation<bool> m_penalized_constraints;
    std::unique_ptr<OptimizerFactory> m_optimizer;
    std::optional<std::pair<bool, double>> m_rescaling;
    std::unique_ptr<PenaltyUpdate> m_penalty_update;
    std::optional<SolutionStatus> m_feasible_solution_status;
    std::optional<double> m_initial_penalty_parameter;
};

idol::PenaltyMethod operator+(const idol::PenaltyMethod& t_penalty_method, const idol::OptimizerFactory& t_optimizer_factory);

#endif //IDOL_PENALTYMETHOD_H
