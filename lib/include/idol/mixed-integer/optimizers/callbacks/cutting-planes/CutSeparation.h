//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H
#define CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/general/utils/GenerationPattern.h"

namespace idol::impl {
    class CutSeparation;
}

class idol::impl::CutSeparation : public Callback {
    CallbackEvent m_triggering_event;
    std::unique_ptr<Model> m_separation_problem;
    GenerationPattern<Ctr> m_cut_pattern;
    GenerationPattern<Ctr> m_separation_objective_pattern;
    CtrType m_type;

    double m_tolerance = idol::Tolerance::Feasibility;
protected:
    void operator()(CallbackEvent t_event) override;

    virtual void hook_add_cut(const TempCtr& t_cut) = 0;

    [[nodiscard]] virtual std::string name() const = 0;
public:
    explicit CutSeparation(CallbackEvent t_triggering_event,
                           Model* t_separation_problem,
                           GenerationPattern<Ctr> t_cut,
                           CtrType t_type);
};

#endif //CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H
