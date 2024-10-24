//
// Created by henri on 12/04/23.
//

#ifndef IDOL_CALLBACKASBRANCHANDBOUNDCALLBACK_H
#define IDOL_CALLBACKASBRANCHANDBOUNDCALLBACK_H

#include <memory>
#include "idol/optimizers/mixed-integer-optimization/callbacks/CallbackFactory.h"
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
                return m_parent.original_model();
            }

            void add_user_cut(const TempCtr &t_cut) override {
                m_parent.add_user_cut(t_cut);
            }

            void add_lazy_cut(const TempCtr &t_cut) override {
                m_parent.add_lazy_cut(t_cut);
            }

            void submit_heuristic_solution(PrimalPoint t_solution) override {
                auto* info = new NodeInfoT();
                info->set_primal_solution(std::move(t_solution));
                m_parent.submit_heuristic_solution(info);
            }

            [[nodiscard]] PrimalPoint primal_solution() const override {
                return m_parent.node().info().primal_solution();
            }

            [[nodiscard]] const Timer &time() const override {
                return m_parent.time();
            }

            double best_obj() const override {
                return m_parent.best_obj();
            }

            double best_bound() const override {
                return m_parent.best_bound();
            }

            void terminate() override {
                m_parent.terminate();
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
