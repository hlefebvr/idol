//
// Created by henri on 19.06.24.
//
#include "idol/optimizers/mixed-integer-programming/wrappers/Mosek/MosekCallbackI.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/Mosek/Optimizers_Mosek.h"

#ifdef IDOL_USE_MOSEK

idol::MosekCallbackI::MosekCallbackI(Optimizers::Mosek &t_parent) : m_parent(t_parent) {

}

void idol::MosekCallbackI::callback(MSKcallbackcodee t_caller,
                                    const double *t_double_info,
                                    const int32_t *t_int32_info,
                                    const int64_t *t_int64_info) {

    CallbackEvent event;

    switch (t_caller) {
        case MSK_CALLBACK_NEW_INT_MIO: event = IncumbentSolution; break;
        default: return;
    }

    m_double_info = t_double_info;
    m_int32_info = t_int32_info;
    m_int64_info = t_int64_info;

    try {

        call(event);

    } catch (...) {
        m_double_info = nullptr;
        m_int32_info = nullptr;
        m_int64_info = nullptr;
        throw;
    }

    m_double_info = nullptr;
    m_int32_info = nullptr;
    m_int64_info = nullptr;

}

void idol::MosekCallbackI::call(idol::CallbackEvent t_event) {

    for (auto& cb : m_callbacks) {
        execute(*cb, t_event);
    }

}

void idol::MosekCallbackI::add_callback(idol::Callback *t_callback) {
    m_callbacks.emplace_back(t_callback);
}

const idol::Model &idol::MosekCallbackI::original_model() const {
    return m_parent.parent();
}

void idol::MosekCallbackI::add_user_cut(const idol::TempCtr &t_cut) {
    throw Exception("Mosek does not support user cuts.");
}

void idol::MosekCallbackI::add_lazy_cut(const idol::TempCtr &t_cut) {
    throw Exception("Mosek does not support lazy cuts.");
}

void idol::MosekCallbackI::submit_heuristic_solution(idol::Solution::Primal t_solution) {
    throw Exception("Mosek does not support heuristic solutions.");
}

idol::Solution::Primal idol::MosekCallbackI::primal_solution() const {
    throw Exception("Mosek does not support accessing primal solutions in callback.");
}

const idol::Timer &idol::MosekCallbackI::time() const {
    return m_parent.time();
}

double idol::MosekCallbackI::best_obj() const {
    return m_int32_info[MSK_IINF_MIO_NUM_INT_SOLUTIONS] > 0 ? m_double_info[MSK_DINF_MIO_OBJ_INT] : Inf;
}

double idol::MosekCallbackI::best_bound() const {
    return m_int32_info[MSK_IINF_MIO_NUM_RELAX] > 0 ? m_double_info[MSK_DINF_MIO_OBJ_BOUND] : -Inf;
}

void idol::MosekCallbackI::terminate() {
    m_parent.terminate();
}

#endif
