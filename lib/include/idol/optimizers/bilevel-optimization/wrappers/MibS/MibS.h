//
// Created by henri on 01.02.24.
//

#ifndef IDOL_MIBS_H
#define IDOL_MIBS_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/modeling/constraints/Ctr.h"

namespace idol::Bilevel {
    class MibS;
}

class idol::Bilevel::MibS : public OptimizerFactoryWithDefaultParameters<MibS> {
    const Annotation<Var, unsigned int> m_follower_variables;
    const Annotation<Ctr, unsigned int> m_follower_constraints;
    const Ctr m_follower_objective;
public:
    MibS(const Annotation<Var, unsigned int>& t_follower_variables,
         const Annotation<Ctr, unsigned int>& t_follower_constraints,
         Ctr  t_follower_objective);

    MibS(const MibS&) = default;
    MibS(MibS&&) noexcept = default;

    MibS& operator=(const MibS&) = delete;
    MibS& operator=(MibS&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    MibS *clone() const override;
};


#endif //IDOL_MIBS_H
