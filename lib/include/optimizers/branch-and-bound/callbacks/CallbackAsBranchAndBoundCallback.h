//
// Created by henri on 12/04/23.
//

#ifndef IDOL_CALLBACKASBRANCHANDBOUNDCALLBACK_H
#define IDOL_CALLBACKASBRANCHANDBOUNDCALLBACK_H

#include <memory>
#include "optimizers/callbacks/CallbackFactory.h"
#include "BranchAndBoundCallbackFactory.h"
#include "BranchAndBoundCallback.h"

namespace idol {
    template<class NodeInfoT>
    class CallbackAsBranchAndBoundCallback;
}

template<class NodeInfoT>
class idol::CallbackAsBranchAndBoundCallback : public BranchAndBoundCallbackFactory<NodeInfoT> {
    std::unique_ptr<CallbackFactory> m_callback_factory;

    CallbackAsBranchAndBoundCallback(const CallbackAsBranchAndBoundCallback& t_src)
        : m_callback_factory(t_src.m_callback_factory->clone()) {}
public:

    class Strategy : public BranchAndBoundCallback<NodeInfoT> {

        friend class ::idol::CallbackAsBranchAndBoundCallback<NodeInfoT>;

        class Interface : public CallbackI {
            friend class Strategy;

            Strategy& m_parent;
        protected:
            explicit Interface(Strategy& t_parent) : m_parent(t_parent) {}

            [[nodiscard]] const Model &original_model() const override {
                return m_parent.m_interface.original_model();
            }

            void add_user_cut(const TempCtr &t_cut) override {
                m_parent.add_user_cut(t_cut);
            }

            void add_lazy_cut(const TempCtr &t_cut) override {
                m_parent.add_lazy_cut(t_cut);
            }

            void submit_solution(const Solution::Primal &t_solution) override {
                m_parent.submit_solution(t_solution);
            }

            [[nodiscard]] Solution::Primal primal_solution() const override {
                return m_parent.node().info().primal_solution();
            }

            [[nodiscard]] const Timer &time() const override {
                return m_parent.time();
            }

        };

        Interface m_interface;
        std::unique_ptr<Callback> m_callback;
    protected:
        explicit Strategy(Callback* t_cb) : m_callback(t_cb), m_interface(*this) {}

        void operator()(CallbackEvent t_event) override {
            m_interface.execute(*m_callback, t_event);
        }
    };

    explicit CallbackAsBranchAndBoundCallback(const CallbackFactory& t_factory)
        : m_callback_factory(t_factory.clone()) {}

    BranchAndBoundCallback<NodeInfoT> *operator()() override {
        auto* cb = m_callback_factory->operator()();
        return new Strategy(cb);
    }

    BranchAndBoundCallbackFactory<NodeInfoT> *clone() const override {
        return new CallbackAsBranchAndBoundCallback<NodeInfoT>(*this);
    }
};

#endif //IDOL_CALLBACKASBRANCHANDBOUNDCALLBACK_H
