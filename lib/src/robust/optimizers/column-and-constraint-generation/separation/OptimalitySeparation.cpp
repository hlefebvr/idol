//
// Created by Henri on 21/01/2026.
//
#include "idol/robust/optimizers/column-and-constraint-generation/separation/OptimalitySeparation.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"
#include "idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

idol::Robust::CCG::OptimalitySeparation::OptimalitySeparation(const OptimalitySeparation& t_src)
    : Separation(t_src),
      m_bilevel_optimizer(t_src.m_bilevel_optimizer ? t_src.m_bilevel_optimizer->clone() : nullptr) {

}

idol::Robust::CCG::OptimalitySeparation& idol::Robust::CCG::OptimalitySeparation::with_bilevel_optimizer(const OptimizerFactory& t_optimizer) {

    if (m_bilevel_optimizer) {
        throw Exception("A bilevel optimizer has already been set.");
    }

    m_bilevel_optimizer.reset(t_optimizer.clone());

    if (!dynamic_cast<idol::Bilevel::OptimizerInterface*>(m_bilevel_optimizer.get())) {
        std::cout << "WARNING: the bilevel optimizer you have provided does not implement the bilevel interface. "
                     "We are assuming you know what you are doing. If this is not intended, "
                     "you most likely are doing something wrong."
        << std::endl;
    }

    return *this;
}

void idol::Robust::CCG::OptimalitySeparation::operator()() {

    auto [model, description] = build_separation_problem();

    // Set optimizer
    const auto& formulation = get_formulation();
    if (formulation.is_adjustable_robust_problem()) {
        m_bilevel_optimizer->as<Bilevel::OptimizerInterface>().set_bilevel_description(description);
        model.use(*m_bilevel_optimizer);
    } else {
        description.set_lower_level_obj(get_bilevel_description().lower_level_obj());
        model.use(Bilevel::PessimisticAsOptimistic(description) + *m_bilevel_optimizer);
    }

    // Set optimizer parameters
    const auto& parent = get_parent();
    model.optimizer().set_param_time_limit(parent.get_remaining_time());

    // Optimize
    model.optimize();

    const auto status = model.get_status();
    const auto reason = model.get_reason();
    set_status(status, reason);

    if (status != Feasible && status != Optimal) {
        terminate();
        return;
    }

    if (status == Optimal) {
        submit_upper_bound(-model.get_best_obj());
    }

    auto scenario = save_primal(get_robust_description().uncertainty_set(), model);
    add_scenario(std::move(scenario));
}
