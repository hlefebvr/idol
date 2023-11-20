//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H
#define CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H

#include "idol/modeling/models/Model.h"
#include "idol/optimizers/callbacks/Callback.h"

namespace idol::impl {
    class CutSeparation;
}

class idol::impl::CutSeparation : public Callback {
    LogLevel m_log_level;
    Color m_log_color;

    CallbackEvent m_triggering_event;
    std::unique_ptr<Model> m_separation_problem;
    TempCtr m_cut;

    double m_tolerance = idol::Tolerance::Feasibility;
protected:
    std::pair<::idol::Expr<Var, Var>, ObjectiveSense> create_separation_objective(const Solution::Primal& t_primal_solution);

    void operator()(CallbackEvent t_event) override;

    virtual void hook_add_cut(const TempCtr& t_cut) = 0;

    [[nodiscard]] LogLevel log_level() const { return m_log_level; }

    [[nodiscard]] Color log_color() const { return m_log_color; }

    [[nodiscard]] virtual std::string name() const = 0;
public:
    explicit CutSeparation(CallbackEvent t_triggering_event,
                           Model* t_separation_problem,
                           TempCtr t_cut,
                           LogLevel t_level,
                           Color t_color
                           );
};




#endif //CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H
