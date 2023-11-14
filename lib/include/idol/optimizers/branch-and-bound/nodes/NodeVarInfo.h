//
// Created by henri on 18.10.23.
//

#ifndef IDOL_NODEVARINFO_H
#define IDOL_NODEVARINFO_H

#include "idol/modeling/models/Model.h"
#include "NodeVarUpdator.h"

namespace idol {
    class NodeVarInfo;

    namespace Optimizers {
        template<class NodeInfoT> class BranchAndBound;
    }

}

class idol::NodeVarInfo {
public:
    struct BranchingDecision {
        Var variable;
        CtrType type;
        double bound;
        BranchingDecision(Var  t_variable, CtrType t_type, double t_bound) : variable(std::move(t_variable)), type(t_type), bound(t_bound) {}
    };

    NodeVarInfo() = default;

    virtual ~NodeVarInfo() = default;

    [[nodiscard]] SolutionStatus status() const { return m_primal_solution.status(); }

    [[nodiscard]] SolutionReason reason() const { return m_primal_solution.reason(); }

    [[nodiscard]] bool has_objective_value() const { return m_primal_solution.has_objective_value(); }

    [[nodiscard]] double objective_value() const { return m_primal_solution.objective_value(); }

    [[nodiscard]] double sum_of_infeasibilities() const { return m_sum_of_infeasibilities.value(); }

    [[nodiscard]] const auto& primal_solution() const { return m_primal_solution; }

    void set_primal_solution(Solution::Primal t_primal_solution) { m_primal_solution = std::move(t_primal_solution); }

    virtual void save(const Model& t_original_formulation, const Model& t_model);

    [[nodiscard]] virtual NodeVarInfo* create_child() const;

    void set_local_lower_bound(const Var& t_var, double t_lb);

    void set_local_upper_bound(const Var& t_var, double t_ub);

    bool has_branching_decision() const { return m_branching_decision.has_value(); }

    [[nodiscard]] const BranchingDecision& branching_decision() const { return m_branching_decision.value(); }

    static NodeVarUpdator<NodeVarInfo>* create_updator(Model& t_relaxation);
private:
    Solution::Primal m_primal_solution;
    std::optional<double> m_sum_of_infeasibilities;
    std::optional<BranchingDecision> m_branching_decision;
};

#endif //IDOL_NODEVARINFO_H
