//
// Created by henri on 07/09/22.
//
#include <utility>

#include "../../../include/modeling/constraints/Ctr.h"
#include "../../../include/modeling/constraints/impl_Constraint.h"
#include "../../../include/modeling/solutions/Solution.h"
#include "../../../include/modeling/environment/Env.h"

std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr) {
    return t_os << t_ctr.name();
}