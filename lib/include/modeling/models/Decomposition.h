//
// Created by henri on 06/02/23.
//

#ifndef IDOL_DECOMPOSITION_H
#define IDOL_DECOMPOSITION_H

#include "modeling/models/Model.h"

static const unsigned int MasterId = std::numeric_limits<unsigned int>::max();

template<typename AxisT>
class Decomposition {
    static_assert(std::is_same_v<AxisT, Var> || std::is_same_v<AxisT, Ctr>);
    using OppositeAxisT = std::conditional_t<std::is_same_v<AxisT, Var>, Ctr, Var>;

    Annotation<AxisT, unsigned int> m_axis_subproblem;
    Annotation<OppositeAxisT, unsigned int> m_opposite_axis_subproblem;
    std::vector<std::optional<AxisT>> m_epigraphs;
    std::vector<Model> m_subproblems;
    Model m_master_problem;
public:
    Decomposition(Env& t_env, unsigned int t_n_subproblems);

    [[nodiscard]] unsigned int n_subproblems() const { return m_subproblems.size(); }

    auto& master_problem() { return m_master_problem; }

    [[nodiscard]] const auto& master_problem() const { return m_master_problem; }

    auto& subproblem(unsigned int t_index) { return m_subproblems[t_index]; }

    [[nodiscard]] const auto& subproblem(unsigned int t_index) const { return m_subproblems[t_index]; }

    auto subproblems() { return IteratorForward(m_subproblems); }

    [[nodiscard]] auto subproblems() const { return ConstIteratorForward(m_subproblems); }

    [[nodiscard]] bool has_epigraph(unsigned int t_index) const { return m_epigraphs[t_index].has_value(); }

    auto& epigraph(unsigned int t_index) { return m_epigraphs[t_index]; }

    [[nodiscard]] const auto& epigraph(unsigned int t_index) const { return m_epigraphs[t_index]; }

    auto epigraphs() { return IteratorForward(m_epigraphs); }

    [[nodiscard]] auto epigraphs() const { return ConstIteratorForward(m_epigraphs); }

    [[nodiscard]] const auto& axis_annotation() const { return m_axis_subproblem; }

    [[nodiscard]] const auto& opposite_axis_annotation() const { return m_opposite_axis_subproblem; }

    void set_epigraph(unsigned int t_index, const AxisT& t_axis);
};

template class Decomposition<Var>;
template class Decomposition<Ctr>;

#endif //IDOL_DECOMPOSITION_H
