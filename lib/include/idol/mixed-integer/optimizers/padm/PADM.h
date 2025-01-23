//
// Created by henri on 18.09.24.
//

#ifndef IDOL_PADM_H
#define IDOL_PADM_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "idol/general/utils/Map.h"
#include "SubProblem.h"
#include "Formulation.h"
#include "PenaltyUpdates.h"
#include "idol/mixed-integer/optimizers/callbacks/watchers/PlotManager.h"
#include <optional>

namespace idol {
    class PADM;
}

class idol::PADM : public OptimizerFactoryWithDefaultParameters<PADM> {
public:
    explicit PADM(Annotation<unsigned int> t_decomposition);

    PADM(Annotation<unsigned int> t_decomposition, Annotation<double> t_penalized_constraints);

    PADM(const PADM& t_src);
    PADM(PADM&&) = default;

    PADM& operator=(const PADM&) = delete;
    PADM& operator=(PADM&&) = default;

    PADM& with_default_sub_problem_spec(ADM::SubProblem t_sub_problem);

    PADM& with_sub_problem_spec(unsigned int t_id, ADM::SubProblem t_sub_problem);

    PADM& with_rescaling_threshold(double t_threshold);

    PADM& with_penalty_update(const PenaltyUpdate& t_penalty_update);

    PADM& with_feasible_solution_status(SolutionStatus t_status);

    PADM& with_iteration_plot(Plots::Manager& t_manager);

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] OptimizerFactory *clone() const override;
private:
    Annotation<unsigned int> m_decomposition;
    std::optional<Annotation<double>> m_penalized_constraints;
    std::optional<ADM::SubProblem> m_default_sub_problem_spec;
    Map<unsigned int, ADM::SubProblem> m_sub_problem_specs;
    std::optional<double> m_rescaling;
    std::unique_ptr<PenaltyUpdate> m_penalty_update;
    std::optional<SolutionStatus> m_feasible_solution_status;
    std::optional<Plots::Manager*> m_plot_manager;

    [[nodiscard]] std::vector<ADM::SubProblem> create_sub_problem_specs(const Model& t_model, const ADM::Formulation& t_formulation) const;
};


#endif //IDOL_PADM_H
