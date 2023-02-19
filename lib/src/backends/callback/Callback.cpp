#include "backends/callback/Callback.h"
#include "backends/branch-and-bound/BranchAndBound.h"

const AbstractModel &impl::Callback::node_model() const {
    return m_parent->relaxed_model();
}

const AbstractModel &impl::Callback::original_model() const {
    return m_parent->parent();
}

bool impl::Callback::submit(Solution::Primal &&t_solution) const {
    return m_parent->submit_solution(std::move(t_solution));
}
