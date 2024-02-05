//
// Created by henri on 01.02.24.
//

#include "MibS.h"

#include <utility>
#include "Optimizers_MibS.h"

idol::MibS::MibS(const idol::Annotation<idol::Var, unsigned int> &t_follower_variables,
                 const idol::Annotation<idol::Ctr, unsigned int> &t_follower_constraints,
                 idol::Ctr t_follower_objective)
        : m_follower_variables(t_follower_variables),
          m_follower_constraints(t_follower_constraints),
          m_follower_objective(std::move(t_follower_objective)) {

}

idol::Optimizer *idol::MibS::operator()(const idol::Model &t_model) const {
    return new Optimizers::MibS(
                t_model,
                m_follower_variables,
                m_follower_constraints,
                m_follower_objective
            );
}

idol::MibS *idol::MibS::clone() const {
    return new MibS(*this);
}
