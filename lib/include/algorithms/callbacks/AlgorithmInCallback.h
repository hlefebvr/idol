//
// Created by henri on 13/10/22.
//

#ifndef IDOL_ALGORITHMINCALLBACK_H
#define IDOL_ALGORITHMINCALLBACK_H

#include "../Algorithm.h"
#include "Callback.h"

class AlgorithmInCallback : public Algorithm {
    bool m_is_in_callback = false;
    Algorithm& m_algorithm;
    Callback::Context* m_context = nullptr;
protected:
    AbstractAttributes &attributes() override { return m_algorithm.attributes(); }
    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_algorithm.attributes(); }
    void execute() override {}
public:
    explicit AlgorithmInCallback(Algorithm& t_rmp_solution_strategy);

    void set_context(Callback::Context* t_ctx) { m_context = t_ctx; }

    void set_in_callback(bool t_is_in_callback) { m_is_in_callback = t_is_in_callback; }

    [[nodiscard]] Solution::Primal primal_solution() const override;

    Ctr add_ctr(TempCtr&& t_temporary_constraint) override;
    using Algorithm::add_ctr;

    void remove(const Ctr &t_constraint) override;

    double get_lb(const Var &t_var) const override;

    double get_ub(const Var &t_var) const override;

    VarType get_type(const Var &t_var) const override;

    const Column &get_column(const Var &t_var) const override;

    const Row &get_row(const Ctr &t_ctr) const override;

    CtrType get_type(const Ctr &t_ctr) const override;

    bool has(const Var &t_var) const override;

    bool has(const Ctr &t_ctr) const override;
};

#endif //IDOL_ALGORITHMINCALLBACK_H
