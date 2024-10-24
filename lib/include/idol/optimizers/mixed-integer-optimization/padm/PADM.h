//
// Created by henri on 18.09.24.
//

#ifndef IDOL_PADM_H
#define IDOL_PADM_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/utils/Map.h"
#include "SubProblem.h"
#include "Formulation.h"
#include "PenaltyUpdates.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/watchers/PlotManager.h"
#include <optional>

namespace idol {
    class PADM;
}

class idol::PADM : public OptimizerFactoryWithDefaultParameters<PADM> {
public:
    explicit PADM(Annotation<Var, unsigned int> t_decomposition);

    PADM(Annotation<Var, unsigned int> t_decomposition, Annotation<Ctr, bool> t_penalized_constraints);

    PADM(const PADM& t_src);
    PADM(PADM&&) = default;

    PADM& operator=(const PADM&) = delete;
    PADM& operator=(PADM&&) = default;

    PADM& with_default_sub_problem_spec(ADM::SubProblem t_sub_problem);

    PADM& with_sub_problem_spec(unsigned int t_id, ADM::SubProblem t_sub_problem);

    PADM& with_rescaling(bool t_rescaling, double t_threshold);

    PADM& with_penalty_update(const PenaltyUpdate& t_penalty_update);

    PADM& with_independent_penalty_update(bool t_value);

    PADM& with_feasible_solution_status(SolutionStatus t_status);

    PADM& with_initial_penalty_parameter(double t_value);

    PADM& with_iteration_plot(Plots::Manager& t_manager);

    Optimizer *operator()(const Model &t_model) const override;

    OptimizerFactory *clone() const override;
private:
    Annotation<Var, unsigned int> m_decomposition;
    std::optional<Annotation<Ctr, bool>> m_penalized_constraints;
    std::optional<ADM::SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, ADM::SubProblem> m_sub_problem_specs;
    std::optional<std::pair<bool, double>> m_rescaling;
    std::unique_ptr<PenaltyUpdate> m_penalty_update;
    std::optional<bool> m_independent_penalty_update;
    std::optional<SolutionStatus> m_feasible_solution_status;
    std::optional<double> m_initial_penalty_parameter;
    std::optional<Plots::Manager*> m_plot_manager;

    std::vector<ADM::SubProblem> create_sub_problem_specs(const Model& t_model, const ADM::Formulation& t_formulation) const;
};


#endif //IDOL_PADM_H
