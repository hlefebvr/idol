//
// Created by henri on 01.02.24.
//

#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"

#include <utility>
#include "idol/bilevel/optimizers/wrappers/MibS/Optimizers_MibS.h"

#ifdef IDOL_USE_CPLEX
#include <OsiCpxSolverInterface.hpp>
#endif

#ifdef IDOL_USE_CLP
#include <OsiClpSolverInterface.hpp>
#endif

idol::Bilevel::MibS::MibS(const Bilevel::Description& t_description)
        : m_description(&t_description) {

}

idol::Optimizer *idol::Bilevel::MibS::operator()(const idol::Model &t_model) const {
#ifdef IDOL_USE_MIBS

    if (!m_description) {
        throw Exception("The bilevel description has not been set.");
    }

    OsiSolverInterface* osi_interface;
    if (m_osi_interface) {
        if (!dynamic_cast<OsiClpSolverInterface*>(m_osi_interface.get())) {
            std::cerr << "Warning: using other interface than OsiClpSolverInterface is not officially supported by MibS." << std::endl;
        }
        osi_interface = m_osi_interface->clone();
    } else {
        osi_interface = new OsiClpSolverInterface();
    }

    auto* result = new Optimizers::Bilevel::MibS(
                t_model,
                *m_description,
                osi_interface,
                m_use_file_interface.value_or(false),
                m_use_cplex_for_feasibility.value_or(false)
            );

    for (auto& cb : m_callbacks) {
        result->add_callback(cb->operator()());
    }

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
    : OptimizerFactoryWithDefaultParameters<MibS>(t_src),
      m_description(t_src.m_description),
      m_use_file_interface(t_src.m_use_file_interface),
      m_use_cplex_for_feasibility(t_src.m_use_cplex_for_feasibility)
#ifdef IDOL_USE_OSI
    , m_osi_interface(t_src.m_osi_interface ? t_src.m_osi_interface->clone() : nullptr)
#endif
{
    for (const auto &cb : t_src.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }
}

idol::Bilevel::MibS &idol::Bilevel::MibS::with_osi_interface(const OsiSolverInterface &t_osi_optimizer) {
#ifdef IDOL_USE_OSI
    if (m_osi_interface) {
        throw Exception("The optimizer has already been set.");
    }

    m_osi_interface.reset(t_osi_optimizer.clone());

    return *this;
#else
    throw Exception("idol was not linked with Osi.");
#endif
}

idol::Bilevel::MibS &idol::Bilevel::MibS::with_file_interface(bool t_value) {

    if (m_use_file_interface) {
        throw Exception("The file interface has already been set.");
    }

    m_use_file_interface = t_value;

    return *this;
}

idol::Bilevel::MibS &idol::Bilevel::MibS::with_cplex_for_feasibility(bool t_value) {

    if (m_use_cplex_for_feasibility) {
        throw Exception("The use of CPLEX for feasibility has already been set.");
    }

    m_use_cplex_for_feasibility = t_value;

    return *this;
}

idol::Bilevel::MibS &idol::Bilevel::MibS::add_callback(const idol::CallbackFactory &t_cb) {
    m_callbacks.emplace_back(t_cb.clone());
    return *this;
}

void idol::Bilevel::MibS::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}
