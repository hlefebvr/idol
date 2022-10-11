//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEMES_SP_H
#define OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEMES_SP_H

#include "ColumnGenerationBranchingScheme.h"

namespace ColumnGenerationBranchingSchemes {
    class SP;
}

class ColumnGenerationSP;
class Var;
class TempCtr;

class ColumnGenerationBranchingSchemes::SP : public ColumnGenerationBranchingScheme {
public:
    SP() = default;

    SP(const SP&) = default;
    SP(SP&&) noexcept = default;

    SP& operator=(const SP&) = delete;
    SP& operator=(SP&&) noexcept = delete;

    void set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) override;

    void set_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSP &t_subproblem) override;

    std::optional<Ctr> contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSP& t_subproblem) override;
};

#endif //OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEMES_SP_H
