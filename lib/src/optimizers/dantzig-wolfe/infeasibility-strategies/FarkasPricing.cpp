//
// Created by henri on 31.10.23.
//
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/dantzig-wolfe/ColumnGeneration.h"

void idol::DantzigWolfe::FarkasPricing::Strategy::execute(Optimizers::DantzigWolfeDecomposition &t_parent) {

    Optimizers::DantzigWolfeDecomposition::ColumnGeneration column_generation(t_parent, true, t_parent.get_param_best_bound_stop());
    column_generation.execute();

    const auto status = column_generation.status();

    set_status(status);
    set_reason(column_generation.reason());
    set_best_obj(column_generation.best_obj());
    set_best_bound(column_generation.best_bound());

    if (status == Optimal || status == Feasible) {
        set_primal_solution(column_generation.primal_solution());
    }

}

