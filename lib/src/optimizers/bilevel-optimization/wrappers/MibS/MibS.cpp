//
// Created by henri on 01.02.24.
//

#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"

#include <utility>
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/Optimizers_MibS.h"

#ifdef IDOL_USE_CPLEX
#include <OsiCpxSolverInterface.hpp>
#endif

idol::Bilevel::MibS::MibS(Bilevel::LowerLevelDescription  t_description)
        : m_description(std::move(t_description)) {

}

idol::Optimizer *idol::Bilevel::MibS::operator()(const idol::Model &t_model) const {
#ifdef IDOL_USE_OSI

    OsiSolverInterface* osi_interface;
    if (m_osi_interface) {
        osi_interface = m_osi_interface->clone();
    } else {
#ifdef IDOL_USE_CPLEX
        osi_interface = new OsiCpxSolverInterface();
#else
        osi_interface = new OsiClpSolverInterface);
#endif
    }


    auto* result = new Optimizers::Bilevel::MibS(
                t_model,
                m_description,
                osi_interface
            );

    this->handle_default_parameters(result);

    return result;
#else
    throw Exception("idol was not linked with MibS.");
#endif
}

idol::Bilevel::MibS *idol::Bilevel::MibS::clone() const {
    return new MibS(*this);
}

idol::Bilevel::MibS::MibS(const idol::Bilevel::MibS &t_src)
    : m_description(t_src.m_description),
      m_osi_interface(t_src.m_osi_interface ? t_src.m_osi_interface->clone() : nullptr) {

}

idol::Bilevel::MibS &idol::Bilevel::MibS::with_optimizer(const OsiSolverInterface &t_osi_optimizer) {

    if (m_osi_interface) {
        throw Exception("The optimizer has already been set.");
    }

    m_osi_interface = std::unique_ptr<OsiSolverInterface>(t_osi_optimizer.clone());

    return *this;
}
