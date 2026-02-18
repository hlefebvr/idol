//
// Created by henri on 18.10.23.
//

#ifndef IDOL_DEFAULTNODEINFO_H
#define IDOL_DEFAULTNODEINFO_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "DefaultNodeUpdator.h"
#include "BranchingDecision.h"

namespace idol {
    class DefaultNodeInfo;

    namespace Optimizers {
        template<class NodeInfoT> class BranchAndBound;
    }
}

class idol::DefaultNodeInfo {
public:
    DefaultNodeInfo() = default;

    virtual ~DefaultNodeInfo() = default;

    [[nodiscard]] SolutionStatus status() const { return m_primal_solution.status(); }

    [[nodiscard]] SolutionReason reason() const { return m_primal_solution.reason(); }

    [[nodiscard]] bool has_objective_value() const { return m_primal_solution.has_objective_value(); }

    [[nodiscard]] double objective_value() const { return m_primal_solution.objective_value(); }

    [[nodiscard]] double sum_of_infeasibilities() const { return m_sum_of_infeasibilities.value(); }

    [[nodiscard]] const auto& primal_solution() const { return m_primal_solution; }

    auto& primal_solution() { return m_primal_solution; }

    void set_primal_solution(PrimalPoint t_primal_solution) { m_primal_solution = std::move(t_primal_solution); }

    virtual void save(const Model& t_original_formulation, const Model& t_model);

    [[nodiscard]] virtual DefaultNodeInfo* create_child() const;

    void add_branching_variable(const Var& t_var, CtrType t_type, double t_bound);

    void add_branching_constraint(const Ctr &t_ctr, TempCtr t_temporary_constraint);

    [[nodiscard]] auto variable_branching_decisions() const { return ConstIteratorForward(m_variable_branching_decisions); }

    [[nodiscard]] auto constraint_branching_decisions() const { return ConstIteratorForward(m_constraint_branching_decisions); }

    static DefaultNodeUpdator<DefaultNodeInfo>* create_updator(const Model& t_src_model, Model& t_relaxation);

    [[nodiscard]] virtual DefaultNodeInfo* clone() const;
protected:
    virtual void compute_sum_of_infeasibilities();
private:
    PrimalPoint m_primal_solution;
    std::optional<double> m_sum_of_infeasibilities;
    std::list<VarBranchingDecision> m_variable_branching_decisions;
    std::list<CtrBranchingDecision> m_constraint_branching_decisions;
};

#endif //IDOL_DEFAULTNODEINFO_H
