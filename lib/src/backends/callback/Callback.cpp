#include "backends/callback/Callback.h"
#include "backends/branch-and-bound/BranchAndBound.h"

#define TEMPORARY_SET(attr_arg, attr_val) \
void HeuristicInterface::set(const Req<attr_val, attr_arg> &t_attr, const attr_arg &t_var, attr_val t_value) { \
    const auto historical_value = m_relaxation.model().get(t_attr, t_var); \
    m_history_##attr_arg##_##attr_val.emplace_front(&t_attr, t_var, historical_value); \
    m_relaxation.model().set(t_attr, t_var, t_value); \
}

#define RESTORE_SET(attr_arg, attr_val) \
for (const auto& [attr, var, val] : m_history_##attr_arg##_##attr_val) { \
    model.set(*attr, var, val); \
}

const Relaxation &impl::Callback::relaxation() const {
    return m_parent->m_relaxations.get();
}

const AbstractModel &impl::Callback::original_model() const {
    return m_parent->parent();
}

bool impl::Callback::submit(Solution::Primal &&t_solution) const {
    return m_parent->submit_solution(std::move(t_solution));
}

HeuristicInterface impl::Callback::temporary_update_session() {
    return { *this, m_parent->m_relaxations.get() };
}

HeuristicInterface::HeuristicInterface(impl::Callback &t_parent, Relaxation& t_relaxation)
    : m_parent(t_parent), m_relaxation(t_relaxation) {

}

TEMPORARY_SET(Var, double);
TEMPORARY_SET(Var, int);

void HeuristicInterface::reoptimize() {
    m_relaxation.model().optimize();
}

HeuristicInterface::~HeuristicInterface() {

    auto& model = m_relaxation.model();

    RESTORE_SET(Var, double);
    RESTORE_SET(Var, int);

}
