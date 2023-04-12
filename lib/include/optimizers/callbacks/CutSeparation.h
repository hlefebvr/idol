//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H
#define CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H

#include "modeling/models/Model.h"
#include "optimizers/callbacks/Callback.h"

namespace impl {
    class CutSeparation;
}

class impl::CutSeparation : public Callback {
    CallbackEvent m_triggering_event;
    std::unique_ptr<Model> m_separation_problem;
    TempCtr m_cut;
protected:
    void operator()(CallbackEvent t_event) override;

    virtual void hook_add_cut(const TempCtr& t_cut) = 0;
public:
    explicit CutSeparation(CallbackEvent t_triggering_event, Model* t_separation_problem, TempCtr t_cut);
};




#endif //CCG_WITH_NESTED_CG_CUTSEPARATIONCALLBACK_H
