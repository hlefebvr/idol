//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H
#define CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H

#include "CutSeparation.h"
#include "CallbackFactory.h"

class LazyCutCallback : public CallbackFactory {
    std::unique_ptr<Model> m_model;
    TempCtr m_cut;

    LazyCutCallback(const LazyCutCallback& t_src) : m_model(t_src.m_model->clone()), m_cut(t_src.m_cut) {}
public:
    LazyCutCallback(const Model& t_model, TempCtr t_cut) : m_model(t_model.clone()), m_cut(std::move(t_cut)) {}

    class Strategy : public impl::CutSeparation {
    protected:
        void hook_add_cut(const TempCtr &t_cut) override {
                add_lazy_cut(t_cut);
        }
    public:
        explicit Strategy(Model* t_separation_problem, TempCtr t_cut)
        : impl::CutSeparation(t_separation_problem, std::move(t_cut)) {}

    };

    Callback *operator()() override {
        return new Strategy(m_model->clone(), m_cut);
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new LazyCutCallback(*this);
    }
};

#endif //CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H
