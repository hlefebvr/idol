//
// Created by henri on 01.02.24.
//

#include "idol/bilevel/optimizers/wrappers/MibS/MibS.h"
#include "idol/bilevel/optimizers/wrappers/MibS/Optimizers_MibS.h"

#ifdef IDOL_USE_OSI_CLP
#include <OsiClpSolverInterface.hpp>
#endif

idol::Bilevel::MibS::MibS(const Bilevel::Description& t_description)
        : m_description(&t_description) {

}

idol::Optimizer *idol::Bilevel::MibS::create(const idol::Model &t_model) const {
#ifdef IDOL_USE_MIBS
    if (!m_description) {
        throw Exception("The bilevel description has not been set.");
    }

    auto* result = new Optimizers::Bilevel::MibS(
                t_model,
                *m_description,
                m_use_file_interface.value_or(false),
                m_native_feasibility_checker.value_or("SYMPHONY"),
                m_feasibility_checker_optimizer ? m_feasibility_checker_optimizer->clone() : nullptr
            );

    for (auto& cb : m_callbacks) {
        result->add_callback(cb->operator()());
    }

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
      m_native_feasibility_checker(t_src.m_native_feasibility_checker),
      m_feasibility_checker_optimizer(t_src.m_feasibility_checker_optimizer ? t_src.m_feasibility_checker_optimizer->clone() : nullptr)
{
    for (const auto &cb : t_src.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }
}

idol::Bilevel::MibS& idol::Bilevel::MibS::with_native_feasibility_checker( const std::string& t_native_feasibility_checker) {

    if (m_feasibility_checker_optimizer) {
        throw Exception("A non-native feasibility checker has already been set.\n"
                        "This is exclusive with a native feasibility checker.");
    }

    if (m_native_feasibility_checker) {
        throw Exception("A native feasibility checker has already been set.");
    }

    m_native_feasibility_checker = t_native_feasibility_checker;

    return *this;
}

idol::Bilevel::MibS& idol::Bilevel::MibS::with_feasibility_checker(const OptimizerFactory& t_feasibility_checker) {

    if (m_native_feasibility_checker) {
        throw Exception("A native feasibility checker has already been set.\n"
                        "This is exclusive with a non-native feasibility checker.");
    }

    if (m_feasibility_checker_optimizer) {
        throw Exception("A feasibility checker has already been set.");
    }

    m_feasibility_checker_optimizer.reset(t_feasibility_checker.clone());

    return *this;
}

idol::Bilevel::MibS &idol::Bilevel::MibS::with_file_interface(bool t_value) {

    if (m_use_file_interface) {
        throw Exception("The file interface has already been set.");
    }

    m_use_file_interface = t_value;

    return *this;
}

idol::Bilevel::MibS &idol::Bilevel::MibS::add_callback(const idol::CallbackFactory &t_cb) {
    m_callbacks.emplace_back(t_cb.clone());
    return *this;
}

void idol::Bilevel::MibS::set_bilevel_description(const idol::Bilevel::Description &t_bilevel_description) {
    m_description = &t_bilevel_description;
}
