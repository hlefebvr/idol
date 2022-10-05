//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_ABSTRACTCUTGENERATOR_H
#define OPTIMIZE_ABSTRACTCUTGENERATOR_H

#include <modeling/expressions/Row.h>
#include <modeling/constraints/TempCtr.h>

namespace Solution {
    class Primal;
    class Dual;
}

class CutGenerationSubproblem;

class AbstractCutGenerator {
    CtrType m_type = GreaterOrEqual;
public:
    virtual ~AbstractCutGenerator() = default;

    virtual Row get_separation_objective(const Solution::Primal& t_primals) = 0;

    [[nodiscard]] virtual TempCtr create_cut(const Solution::Primal& t_primals) const = 0;

    [[nodiscard]] CtrType type() const { return m_type; }

    virtual bool set_lower_bound(const Var& t_var, double t_lb, CutGenerationSubproblem& t_subproblem) = 0;

    virtual bool set_upper_bound(const Var& t_var, double t_lb, CutGenerationSubproblem& t_subproblem) = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution(const CutGenerationSubproblem& t_subproblem, const Solution::Dual& t_rmp_duals) const = 0;

};

#endif //OPTIMIZE_ABSTRACTCUTGENERATOR_H
