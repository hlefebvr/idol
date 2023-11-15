//
// Created by henri on 15.11.23.
//

#ifndef IDOL_BRANCHINGDECISION_H
#define IDOL_BRANCHINGDECISION_H

#include <utility>

#include "idol/modeling/variables/Var.h"
#include "idol/modeling/constraints/Ctr.h"
#include "idol/modeling/constraints/TempCtr.h"

namespace idol {

    struct VarBranchingDecision {
        Var variable;
        CtrType type;
        double bound;
        VarBranchingDecision(Var  t_variable, CtrType t_type, double t_bound) : variable(std::move(t_variable)), type(t_type), bound(t_bound) {}
    };

    struct CtrBranchingDecision {
        Ctr constraint;
        TempCtr temporary_constraint;
        CtrBranchingDecision(Ctr  t_constraint, TempCtr&& t_temporary_constraint) : constraint(std::move(t_constraint)), temporary_constraint(t_temporary_constraint) {}
    };

}

#endif //IDOL_BRANCHINGDECISION_H
