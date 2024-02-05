//
// Created by henri on 01.02.24.
//

#ifndef IDOL_MIBS_H
#define IDOL_MIBS_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/modeling/constraints/Ctr.h"

namespace idol {
    class MibS;
}

class idol::MibS : public OptimizerFactoryWithDefaultParameters<MibS> {
    const idol::Annotation<idol::Var, unsigned int> m_follower_variables;
    const idol::Annotation<idol::Ctr, unsigned int> m_follower_constraints;
    const idol::Ctr m_follower_objective;
public:
    MibS(const idol::Annotation<idol::Var, unsigned int>& t_follower_variables,
         const idol::Annotation<idol::Ctr, unsigned int>& t_follower_constraints,
         idol::Ctr  t_follower_objective);

    MibS(const MibS&) = default;
    MibS(MibS&&) noexcept = default;

    MibS& operator=(const MibS&) = delete;
    MibS& operator=(MibS&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    MibS *clone() const override;
};


#endif //IDOL_MIBS_H
