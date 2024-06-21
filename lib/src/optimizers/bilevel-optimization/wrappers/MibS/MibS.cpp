//
// Created by henri on 01.02.24.
//

#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"

#include <utility>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/Optimizers_MibS.h"

idol::Bilevel::MibS::MibS(Bilevel::LowerLevelDescription  t_description)
        : m_description(std::move(t_description)) {

}

idol::Optimizer *idol::Bilevel::MibS::operator()(const idol::Model &t_model) const {
#ifdef IDOL_USE_OSI
    return new Optimizers::Bilevel::MibS(
                t_model,
                m_description
            );
#else
    throw Exception("idol was not linked with MibS.");
#endif
}

idol::Bilevel::MibS *idol::Bilevel::MibS::clone() const {
    return new MibS(*this);
}
