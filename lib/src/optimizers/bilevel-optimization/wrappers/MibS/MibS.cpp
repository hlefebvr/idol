//
// Created by henri on 01.02.24.
//

#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"

#include <utility>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/Optimizers_MibS.h"

idol::Bilevel::MibS::MibS(const Annotation<Var, unsigned int> &t_follower_variables,
                 const Annotation<Ctr, unsigned int> &t_follower_constraints,
                 Ctr t_follower_objective)
        : m_follower_variables(t_follower_variables),
          m_follower_constraints(t_follower_constraints),
          m_follower_objective(std::move(t_follower_objective)) {

}

idol::Optimizer *idol::Bilevel::MibS::operator()(const idol::Model &t_model) const {
#ifdef IDOL_USE_OSI
    return new Optimizers::Bilevel::MibS(
                t_model,
                m_follower_variables,
                m_follower_constraints,
                m_follower_objective
            );
#else
    throw Exception("idol was not linked with MibS.");
#endif
}

idol::Bilevel::MibS *idol::Bilevel::MibS::clone() const {
    return new MibS(*this);
}
