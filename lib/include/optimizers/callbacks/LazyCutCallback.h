//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H
#define CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H

#include "CutSeparationCallback.h"

class LazyCutCallback : public CutSeparationCallback {
protected:
    void hook_add_cut(const TempCtr &t_cut) override {
        add_lazy_cut(t_cut);
    }
public:
    explicit LazyCutCallback(Model* t_separation_problem, TempCtr t_cut)
        : CutSeparationCallback(t_separation_problem, std::move(t_cut)) {}
};

#endif //CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H
