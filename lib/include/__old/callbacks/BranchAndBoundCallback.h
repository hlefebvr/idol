//
// Created by henri on 17/10/22.
//

#ifndef IDOL_BRANCHANDBOUNDCALLBACK_H
#define IDOL_BRANCHANDBOUNDCALLBACK_H

#include <cassert>
#include "Callback.h"

class BranchAndBound::Callback : public ::Callback {
public:
    class AdvancedContext;

    AdvancedContext& advanced(Context& t_ctx);
    [[nodiscard]] const AdvancedContext& advanced(const Context& t_ctx) const;
};

class BranchAndBound::Callback::AdvancedContext : public ::Callback::Context {
    BranchAndBound& m_parent;
    Event m_event;
    std::list<std::tuple<Var, double, double>> m_temp_fixations;
public:
    AdvancedContext(BranchAndBound& t_parent, Event t_event) : m_parent(t_parent), m_event(t_event) {}

    [[nodiscard]] Event event() const override { return m_event; }

    const Node& node() { return m_parent.current_node(); }

    [[nodiscard]] Solution::Primal node_primal_solution() const override;

    [[nodiscard]] Solution::Primal primal_solution() const;

    void fix_variables(const std::list<std::pair<Var, double>>& t_fixations);

    [[nodiscard]] unsigned int level() const;

    void resolve();

    bool submit_solution(Solution::Primal &&t_solution) override;

    ~AdvancedContext() override;
};

#endif //IDOL_BRANCHANDBOUNDCALLBACK_H
