//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_USERCUTCALLBACK_H
#define CCG_WITH_NESTED_CG_USERCUTCALLBACK_H

#include "CutSeparationCallback.h"

class UserCutCallback : public CutSeparationCallback {
protected:
    void hook_add_cut(const TempCtr &t_cut) override {
        add_user_cut(t_cut);
    }
public:
    explicit UserCutCallback(Model* t_separation_problem, TempCtr t_cut)
            : CutSeparationCallback(t_separation_problem, std::move(t_cut)) {}
};

#endif //CCG_WITH_NESTED_CG_USERCUTCALLBACK_H
