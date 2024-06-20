//
// Created by henri on 05.02.24.
//

#ifndef IDOL_OSI_H
#define IDOL_OSI_H

#include <memory>

#ifdef IDOL_USE_OSI
#include <OsiSolverInterface.hpp>
#include <OsiCpxSolverInterface.hpp>
#include <OsiSymSolverInterface.hpp>
#include <OsiCbcSolverInterface.hpp>
#include <OsiClpSolverInterface.hpp>
#else
struct OsiSolverInterface {
    virtual ~OsiSolverInterface() = default;
    virtual OsiSolverInterface* clone() const = 0;
};
#endif

#define ADD_SHORTCUT(name, osi_interface) \
namespace idol { class name; }                            \
class idol::name : public idol::Osi {               \
public:                    \
    name() : Osi(osi_interface()) {}      \
    static Osi ContinuousRelaxation() { return Osi::ContinuousRelaxation(osi_interface()); }    \
};

#define ADD_FAKE_SHORTCUT(name) \
namespace idol { class name; }  \
class idol::name : public idol::Osi { \
    name() : Osi(#name) {} \
    static Osi ContinuousRelaxation() { return name(); }    \
};

#include "idol/optimizers/OptimizerFactory.h"

namespace idol {
    class Osi;
}

class idol::Osi : public OptimizerFactoryWithDefaultParameters<Osi> {
    std::unique_ptr<OsiSolverInterface> m_solver_interface;
    bool m_continuous_relaxation = false;

    Osi(const OsiSolverInterface& t_solver_interface, bool t_continuous_relaxation);
protected:
    Osi(const std::string& t_solver);
public:
    Osi(const OsiSolverInterface& t_solver_interface);

    Osi(const Osi& t_src);
    Osi(Osi&&) noexcept = default;

    Osi& operator=(const Osi&) = delete;
    Osi& operator=(Osi&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    OsiSolverInterface* create_osi_solver_interface() const;

    static Osi ContinuousRelaxation(const OsiSolverInterface& t_solver_interface);

    [[nodiscard]] Osi *clone() const override;

};

#ifdef IDOL_USE_CPLEX
ADD_SHORTCUT(OsiCplex, OsiCpxSolverInterface)
#else
ADD_FAKE_SHORTCUT(OsiCplex)
#endif

#ifdef IDOL_USE_SYMPHONY
ADD_SHORTCUT(OsiSymphony, OsiSymSolverInterface)
#else
ADD_FAKE_SHORTCUT(OsiSymphony)
#endif

#ifdef IDOL_USE_CBC
ADD_SHORTCUT(OsiCbc, OsiCbcSolverInterface)
#else
ADD_FAKE_SHORTCUT(OsiCbc)
#endif

#ifdef IDOL_USE_CLP
ADD_SHORTCUT(OsiClp, OsiClpSolverInterface)
#else
ADD_FAKE_SHORTCUT(OsiClp)
#endif

#endif //IDOL_OSI_H
