//
// Created by henri on 05.02.24.
//

#ifndef IDOL_OSI_H
#define IDOL_OSI_H

#include <OsiSolverInterface.hpp>
#include "idol/optimizers/OptimizerFactory.h"

namespace idol {
    class Osi;
}

class idol::Osi : public OptimizerFactoryWithDefaultParameters<Osi> {
    std::unique_ptr<OsiSolverInterface> m_solver_interface;
    bool m_continuous_relaxation = false;

    Osi(const OsiSolverInterface& t_solver_interface, bool t_continuous_relaxation);
public:
    Osi(const OsiSolverInterface& t_solver_interface);

    Osi(const Osi& t_src);
    Osi(Osi&&) noexcept = default;

    Osi& operator=(const Osi&) = delete;
    Osi& operator=(Osi&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static Osi ContinuousRelaxation(const OsiSolverInterface& t_solver_interface);

    [[nodiscard]] Osi *clone() const override;
};


#endif //IDOL_OSI_H
