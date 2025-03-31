//
// Created by henri on 11/04/23.
//

#ifndef CCG_WITH_NESTED_CG_USERCUTCALLBACK_H
#define CCG_WITH_NESTED_CG_USERCUTCALLBACK_H

#include "CutSeparation.h"
#include "idol/mixed-integer/optimizers/callbacks/CallbackFactory.h"

namespace idol {
    class UserCutCallback;
}

class idol::UserCutCallback : public CallbackFactory {
    std::unique_ptr<Model> m_model;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    GenerationPattern<Ctr> m_cut;
    CtrType m_type;

    UserCutCallback(const UserCutCallback& t_src)
        : m_model(t_src.m_model->clone()),
          m_cut(t_src.m_cut),
          m_type(t_src.m_type),
          m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr) {}
public:
    UserCutCallback(const Model& t_model, GenerationPattern<Ctr> t_cut, CtrType t_type = LessOrEqual) : m_model(t_model.clone()), m_cut(std::move(t_cut)), m_type(t_type) {}

    class Strategy : public impl::CutSeparation {
    protected:
        void hook_add_cut(const TempCtr &t_cut) override {
            add_user_cut(t_cut);
        }

        [[nodiscard]] std::string name() const override {
            return "user-cut-callback";
        }

    public:
        explicit Strategy(Model* t_separation_problem, GenerationPattern<Ctr> t_cut, CtrType t_type)
                : impl::CutSeparation(InvalidSolution, t_separation_problem, std::move(t_cut), t_type) {}
    };

    Callback *operator()() override {

        if (!m_optimizer_factory) {
            throw Exception("No optimizer has been given.");
        }

        auto* model = m_model->clone();
        model->use(*m_optimizer_factory);

        auto* result = new Strategy(model, m_cut, m_type);

        return result;
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new UserCutCallback(*this);
    }

    UserCutCallback& with_separation_optimizer(const OptimizerFactory& t_optimizer_factory) {

        if (m_optimizer_factory) {
            throw Exception("An optimizer has already been given.");
        }

        m_optimizer_factory.reset(t_optimizer_factory.clone());

        return *this;
    }
};

#endif //CCG_WITH_NESTED_CG_USERCUTCALLBACK_H
