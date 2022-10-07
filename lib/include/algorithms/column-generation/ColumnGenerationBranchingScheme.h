//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEME_H
#define OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEME_H

#include "modeling/numericals.h"
#include "modeling/Types.h"
#include "modeling/variables/TempVar.h"
#include "modeling/constraints/Constraint.h"

namespace Solution {
    class Primal;
    class Dual;
}

class ColumnGenerationSP;
class Var;
class TempCtr;

class ColumnGenerationBranchingScheme {
public:
    ColumnGenerationBranchingScheme() = default;

    virtual ~ColumnGenerationBranchingScheme() = default;

    virtual void set_lower_bound(const Var& t_var, double t_lb, ColumnGenerationSP& t_subproblem) = 0;

    virtual void set_upper_bound(const Var& t_var, double t_lb, ColumnGenerationSP& t_subproblem) = 0;

    virtual std::optional<Ctr> contribute_to_add_constraint(TempCtr& t_temporary_constraint, ColumnGenerationSP& t_subproblem) = 0;
};

#endif //OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEME_H
