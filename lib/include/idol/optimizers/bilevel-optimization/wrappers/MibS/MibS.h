//
// Created by henri on 01.02.24.
//

#ifndef IDOL_MIBS_H
#define IDOL_MIBS_H

#include <memory>
#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/modeling/constraints/Ctr.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"

#ifdef IDOL_USE_CPLEX
#include <OsiSolverInterface.hpp>
#endif

namespace idol::Bilevel {
    class MibS;
}

class OsiSolverInterface;

class idol::Bilevel::MibS : public OptimizerFactoryWithDefaultParameters<MibS> {
    Bilevel::LowerLevelDescription m_description;
#ifdef IDOL_USE_OSI
    std::unique_ptr<OsiSolverInterface> m_osi_interface;
    std::optional<bool> m_use_file_interface;
#endif
public:
    MibS(Bilevel::LowerLevelDescription t_description);

    MibS(const MibS& t_src);
    MibS(MibS&&) noexcept = delete;

    MibS& operator=(const MibS&) = delete;
    MibS& operator=(MibS&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    MibS& with_osi_interface(const OsiSolverInterface& t_osi_optimizer);

    MibS& with_file_interface(bool t_value);

    MibS *clone() const override;
};

#endif //IDOL_MIBS_H
