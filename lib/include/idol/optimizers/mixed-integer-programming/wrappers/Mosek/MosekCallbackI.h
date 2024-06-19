//
// Created by henri on 19.06.24.
//

#ifndef IDOL_MOSEKCALLBACKI_H
#define IDOL_MOSEKCALLBACKI_H

#include <mosek.h>
#include "../../callbacks/Callback.h"

namespace idol {
    namespace Optimizers {
        class Mosek;
    }
    class MosekCallbackI;
}

class idol::MosekCallbackI : public CallbackI {

    Optimizers::Mosek& m_parent;
    std::list<std::unique_ptr<Callback>> m_callbacks;

    // Information from Mosek Callback
    const double *m_double_info = nullptr;
    const int32_t *m_int32_info = nullptr;
    const int64_t *m_int64_info = nullptr;

public:
    explicit MosekCallbackI(Optimizers::Mosek& t_parent);

    void add_callback(Callback* t_callback);

    void callback(MSKcallbackcodee t_caller, const double * t_double_info, const int32_t* t_int32_info, const int64_t* t_int64_info);
protected:
    void call(CallbackEvent t_event);

    const Model &original_model() const override;

    void add_user_cut(const TempCtr &t_cut) override;

    void add_lazy_cut(const TempCtr &t_cut) override;

    void submit_heuristic_solution(Solution::Primal t_solution) override;

    Solution::Primal primal_solution() const override;

    const Timer &time() const override;
};

#endif //IDOL_MOSEKCALLBACKI_H
