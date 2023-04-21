//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H
#define CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H

#include "CutSeparation.h"
#include "CallbackFactory.h"

class LazyCutCallback : public CallbackFactory {
    std::unique_ptr<Model> m_model;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    TempCtr m_cut;

    LazyCutCallback(const LazyCutCallback& t_src)
        : m_model(t_src.m_model->clone()),
          m_cut(t_src.m_cut),
          m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr) {}
public:
    LazyCutCallback(const Model& t_model, TempCtr t_cut) : m_model(t_model.clone()), m_cut(std::move(t_cut)) {}

    class Strategy : public impl::CutSeparation {
    protected:
        void hook_add_cut(const TempCtr &t_cut) override {
                add_lazy_cut(t_cut);
        }
    public:
        explicit Strategy(Model* t_separation_problem, TempCtr t_cut)
                : impl::CutSeparation(IncumbentSolution, t_separation_problem, std::move(t_cut)) {}

    };

    Callback *operator()() override {

        if (!m_optimizer_factory) {
            throw Exception("No optimizer has been given.");
        }

        auto* model = m_model->clone();
        model->use(*m_optimizer_factory);

        auto* result = new Strategy(model, m_cut);

        return result;
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new LazyCutCallback(*this);
    }

    LazyCutCallback& with_separation_optimizer(const OptimizerFactory& t_optimizer_factory) {

        if (m_optimizer_factory) {
            throw Exception("An optimizer has already been given.");
        }

        m_optimizer_factory.reset(t_optimizer_factory.clone());

        return *this;
    }
};

#endif //CCG_WITH_NESTED_CG_LAZYCUTCALLBACK_H
