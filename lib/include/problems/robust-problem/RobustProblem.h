//
// Created by henri on 02/03/23.
//

#ifndef IDOL_ROBUSTPROBLEM_H
#define IDOL_ROBUSTPROBLEM_H

#include "modeling/models/Model.h"

IDOL_CREATE_PARAMETER_CLASS(AdjustableRobust)

IDOL_CREATE_PARAMETER_TYPE(AdjustableRobust, bool, 4)

IDOL_CREATE_PARAMETER(AdjustableRobust, bool, 0, ConstraintUncertainty, true)
IDOL_CREATE_PARAMETER(AdjustableRobust, bool, 1, CompleteRecourse, true)
IDOL_CREATE_PARAMETER(AdjustableRobust, bool, 2, QuasiConvexityInFirstStage, false)
IDOL_CREATE_PARAMETER(AdjustableRobust, bool, 3, QuasiConvexityInUncertainty, false)

class RobustProblem {
    std::vector<Model> m_stages;
    std::vector<Model> m_uncertainty_sets;
    Param::AdjustableRobust::values<bool> m_bool_parameters;
public:
    explicit RobustProblem(Env& t_env, unsigned int t_n_stages = 1);

    Model& stage(unsigned int t_stage) { return m_stages[t_stage]; }
    [[nodiscard]] const Model& stage(unsigned int t_stage) const { return m_stages[t_stage]; }

    Model& uncertainty_set(unsigned int t_stage = 0) { return m_stages[t_stage]; }
    [[nodiscard]] const Model& uncertainty_set(unsigned int t_stage = 0) const { return m_stages[t_stage]; }

    auto stages() { return IteratorForward(m_stages); }
    [[nodiscard]] auto stages() const { return ConstIteratorForward(m_stages); }

    auto uncertainty_sets() { return IteratorForward(m_uncertainty_sets); }
    [[nodiscard]] auto uncertainty_sets() const { return ConstIteratorForward(m_uncertainty_sets); }

    [[nodiscard]] unsigned int n_stages() const { return m_stages.size(); }

    [[nodiscard]] bool get(const Parameter<bool>& t_param) const;
    void set(const Parameter<bool>& t_param, bool t_value);
};

#endif //IDOL_ROBUSTPROBLEM_H
