//
// Created by henri on 01.02.24.
//

#ifndef IDOL_MIBS_H
#define IDOL_MIBS_H

#include <memory>
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/mixed-integer/optimizers/callbacks/CallbackFactory.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"

#ifdef IDOL_USE_OSI
#include <OsiSolverInterface.hpp>
#endif

namespace idol::Bilevel {
    class MibS;
}

class OsiSolverInterface;

class idol::Bilevel::MibS : public OptimizerFactoryWithDefaultParameters<MibS>, public Bilevel::OptimizerInterface {
    const Bilevel::Description* m_description = nullptr;
    std::optional<bool> m_use_file_interface;
    std::optional<bool> m_use_cplex_for_feasibility;
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;
#ifdef IDOL_USE_OSI
    std::unique_ptr<OsiSolverInterface> m_osi_interface;
#endif
public:
    MibS() = default;

    explicit MibS(const Bilevel::Description& t_description);

    MibS(const MibS& t_src);
    MibS(MibS&&) noexcept = delete;

    MibS& operator=(const MibS&) = delete;
    MibS& operator=(MibS&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    void set_bilevel_description(const Description &t_bilevel_description) override;

    //MibS& with_osi_interface(const OsiSolverInterface& t_osi_optimizer);

    MibS& with_cplex_for_feasibility(bool t_value);

    MibS& with_file_interface(bool t_value);

    MibS& add_callback(const CallbackFactory& t_cb);

    [[nodiscard]] MibS *clone() const override;
};

#endif //IDOL_MIBS_H
