//
// Created by henri on 21.11.24.
//

#ifndef IDOL_DUALIZER_H
#define IDOL_DUALIZER_H

#include <vector>
#include <variant>
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/mixed-integer/modeling/constraints/QCtr.h"

namespace idol {
    class Dualizer;
    class Model;
}

class idol::Dualizer {
    const Model& m_parent;

    std::vector<Var> m_dual_variables; // constraint - qconstraint index <-> dual variable
    std::vector<std::variant<Ctr, QCtr>> m_dual_constraints; // variable index <-> dual constraint
public:
    explicit Dualizer(const Model& t_parent);
};

#endif //IDOL_DUALIZER_H
