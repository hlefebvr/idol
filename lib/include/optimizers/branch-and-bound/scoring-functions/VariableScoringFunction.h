//
// Created by henri on 16.10.23.
//

#ifndef IDOL_VARIABLESCORINGFUNCTION_H
#define IDOL_VARIABLESCORINGFUNCTION_H

#include <list>
#include "modeling/variables/Var.h"
#include "modeling/solutions/Solution.h"

namespace idol {
    class VariableScoringFunction;
}

class idol::VariableScoringFunction {
public:
    virtual ~VariableScoringFunction() = default;

    virtual std::list<double> operator()(const std::list<Var>& t_variables, const Solution::Primal& t_primal_values) = 0;
};

#endif //IDOL_VARIABLESCORINGFUNCTION_H
