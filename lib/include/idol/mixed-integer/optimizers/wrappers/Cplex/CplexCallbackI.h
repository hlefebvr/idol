//
// Created by henri on 10.04.25.
//

#ifndef IDOL_CPLEXCALLBACKI_H
#define IDOL_CPLEXCALLBACKI_H

#ifdef IDOL_USE_CPLEX
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include <ilcplex/ilocplex.h>

namespace idol {
    namespace Optimizers {
        class Cplex;
        namespace impl {
            class CplexUserCutCallbackI;
            class CplexLazyConstraintCallbackI;
            class CplexHeuristicCallbackI;
        }
    }
    class CplexCallbackI;
}

class idol::Optimizers::impl::CplexUserCutCallbackI : public IloCplex::UserCutCallbackI {
    CplexCallbackI *m_callback;
public:
    explicit CplexUserCutCallbackI(IloEnv t_env, CplexCallbackI *t_callback) : IloCplex::UserCutCallbackI(t_env), m_callback(t_callback) {}

    using IloCplex::UserCutCallbackI::add;
protected:
    void main() override;

    [[nodiscard]] CallbackI *duplicateCallback() const override {
        throw Exception("Duplicate CplexUserCutCallbackI!");
    }
};

class idol::Optimizers::impl::CplexLazyConstraintCallbackI : public IloCplex::LazyConstraintCallbackI {
    CplexCallbackI *m_callback = nullptr;
public:
    explicit CplexLazyConstraintCallbackI(IloEnv t_env, CplexCallbackI *t_callback) : IloCplex::LazyConstraintCallbackI(t_env), m_callback(t_callback) {}

    using IloCplex::LazyConstraintCallbackI::add;
protected:
    void main() override;

    [[nodiscard]] CallbackI *duplicateCallback() const override {
        std::cerr << "Warning: Cplex called duplicateCallback and it is not implemented..." << std::endl;
        throw Exception("Duplicate CplexLazyConstraintCallbackI!");
    }
};

class idol::CplexCallbackI : public CallbackI {
    Optimizers::Cplex &m_parent;
    std::list<std::unique_ptr<Callback>> m_callbacks;
    IloCplex::ControlCallbackI* m_caller = nullptr;

    std::unique_ptr<Optimizers::impl::CplexUserCutCallbackI> m_user_cut_callback;
    std::unique_ptr<Optimizers::impl::CplexLazyConstraintCallbackI> m_lazy_constraint_callback;
    // std::unique_ptr<Optimizers::impl::CplexHeuristicCallbackI> m_heuristic_callback;

    Optimizers::impl::CplexUserCutCallbackI* create_user_cut_callback();
    Optimizers::impl::CplexLazyConstraintCallbackI* create_lazy_constraint_callback();
    Optimizers::impl::CplexHeuristicCallbackI* create_heuristic_callback();

    IloRange cplex_temp_constr(const TempCtr &t_temp_ctr);
public:
    explicit CplexCallbackI(Optimizers::Cplex &t_parent);

    void add_callback(Callback *t_callback);

    void call(IloCplex::ControlCallbackI* t_caller, CallbackEvent t_event);

    [[nodiscard]] const Model &original_model() const override;

    void add_lazy_cut(const TempCtr &t_lazy_cut) override;

    void add_user_cut(const TempCtr &t_user_cut) override;

    void submit_heuristic_solution(PrimalPoint t_solution) override {}

    [[nodiscard]] PrimalPoint primal_solution() const override;

    [[nodiscard]] const Timer &time() const override;

    [[nodiscard]] double best_obj() const override;

    [[nodiscard]] double best_bound() const override;

    void terminate() override;

    friend class idol::Optimizers::Cplex;
};

#endif
#endif //IDOL_CPLEXCALLBACKI_H
