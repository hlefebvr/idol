//
// Created by henri on 01.02.24.
//

#ifndef IDOL_MIBS_H
#define IDOL_MIBS_H

#include <memory>
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/mixed-integer/optimizers/callbacks/CallbackFactory.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"

namespace idol::Bilevel {
    class MibS;
}

class OsiSolverInterface;

class idol::Bilevel::MibS : public OptimizerFactoryWithDefaultParameters<MibS>, public Bilevel::OptimizerInterface {
    const Bilevel::Description* m_description = nullptr;
    std::optional<bool> m_use_file_interface;
    std::optional<bool> m_use_cplex_for_feasibility;
    std::list<std::unique_ptr<CallbackFactory>> m_callbacks;
    void* m_osi_interface = nullptr;
public:
    MibS() = default;

    explicit MibS(const Bilevel::Description& t_description);

    MibS(const MibS& t_src);
    MibS(MibS&&) noexcept = delete;

    MibS& operator=(const MibS&) = delete;
    MibS& operator=(MibS&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    void set_bilevel_description(const Description &t_bilevel_description) override;

    MibS& with_osi_interface(const void* t_osi_optimizer);

    MibS& with_cplex_for_feasibility(bool t_value);

    MibS& with_file_interface(bool t_value);

    MibS& add_callback(const CallbackFactory& t_cb);

    [[nodiscard]] MibS *clone() const override;
};

#endif //IDOL_MIBS_H
